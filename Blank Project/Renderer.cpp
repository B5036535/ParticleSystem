#include "Renderer.h"
#include <algorithm>
#include <ctime>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0, 0, Vector3(0, 0, 10));

	triangle = Mesh::GenerateTriangle();
	quad = Mesh::GenerateQuad();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	box = Mesh::LoadFromMeshFile("Cube.msh");
	screenQuad = Mesh::GenerateQuad();
	float offset = 10.f;
	int i = 0;
	for (int x = 0; x < NUM_OF_X; x++)
	{
		for (int y = 0; y < NUM_OF_Y; y++)
		{
			for (int z = 0; z < NUM_OF_Z; z++)
			{
				Vector4 colour = Vector4(std::min((float) x / (float)NUM_OF_X, 1.0f) , std::min((float)y / (float)NUM_OF_Y , 1.0f), 0.f, 1.0f - (float) z / (float) NUM_OF_Z);
				//Vector4 colour = Vector4((float)rand() / (float)RAND_MAX, (float) rand() / (float) RAND_MAX, (float) rand() / (float) RAND_MAX,(float) z / (float) NUM_OF_Z);
				//Vector4 colour = Vector4(1, 0, 0, 1);
				quads[i] = Mesh::GenerateQuad();
				quads[i]->UpdateColour(colour);
				i++;
			}
		}
	}



	basicShader = new Shader("vertex_basic.glsl", "fragment_basic.glsl");
	shader_opaque = new Shader("vertex_opaque.glsl", "fragment_opaque.glsl");
	OITShader = new Shader("vertex_basicOIT.glsl", "fragment_OIT.glsl");
	finalPassShader = new Shader("vertex_OITFinalPass.glsl", "fragment_OITFinalPass.glsl");
	shader_noise = new Shader("vertex_OITFinalPass.glsl", "fragment_noise.glsl");


	GenerateScreenTexture(tex_opaque);
	GenerateScreenTexture(tex_depth_exponential, true);
	GenerateScreenTexture(tex_depth_linear);
	GenerateScreenTexture(tex_normal);

	GenerateScreenTexture(tex_accumulation);
	GenerateScreenTexture(tex_reveal);



	glGenFramebuffers(1, &FBO_opaque);
	GLenum OpaqueBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_opaque);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_opaque, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_normal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tex_depth_linear, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_depth_exponential, 0);
	glDrawBuffers(3, OpaqueBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &FBO_OIT);
	GLenum OITbuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_OIT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_accumulation, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_reveal, 0);
	glDrawBuffers(2, OITbuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	glGenTextures(1, &tex_noise);
	glBindTexture(GL_TEXTURE_2D, tex_noise);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = GL_RGBA16;
	GLuint tType = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, 256, 256, 0, tType, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(1, &FBO_noise);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_noise);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_noise, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	//basicComputeShader = new ComputeShader("compute_basic.glsl");

	psData_A = new ParticleSystemData(quad, 100000, 5.f, 5.0f, EmitterType::SPHERE, Vector3(3,3,3), tex_depth_linear, tex_normal);
	psData_A->FillInitialForceAndVelocity(Vector3(), Vector3(50.f, 1.f, 0.f));
	Vector2 array_colour_r[8] = 
	{
			Vector2(-0.1f, 0.0f),
			Vector2(0.f, 0.0f),
			Vector2(0.2f, 0.2f),
			Vector2(0.5f, 1.0f),
			Vector2(0.6f, 0.7f),
			Vector2(0.75f, 0.2f),
			Vector2(1.0f, 0.3f),
			Vector2(1.85f, 0.3f),
	};
	Spline spline_colour_r = Spline (array_colour_r);

	Vector2 array_colour_g[8] =
	{
			Vector2(-0.1f, 0.0f),
			Vector2(0.f, 1.0f),
			Vector2(0.2f, 0.8f),
			Vector2(0.5f, 0.2f),
			Vector2(0.6f, 0.7f),
			Vector2(0.75f, 0.5f),
			Vector2(1.0f, 0.6f),
			Vector2(1.85f, 0.3f),
	};
	Spline spline_colour_g = Spline(array_colour_g);

	Vector2 array_colour_b[8] =
	{
			Vector2(-0.1f, 0.5f),
			Vector2(0.f, 0.5f),
			Vector2(0.2f, 0.5f),
			Vector2(0.5f, 1.0f),
			Vector2(0.6f, 0.1f),
			Vector2(0.75f, 0.0f),
			Vector2(1.0f, 0.7f),
			Vector2(1.85f, 0.7f),
	};
	Spline spline_colour_b = Spline(array_colour_b);

	Vector2 array_colour_a[8] =
	{
			Vector2(-0.1f, 0.5f),
			Vector2(0.f, 1.0f),
			Vector2(0.2f, 1.0f),
			Vector2(0.5f, 1.0f),
			Vector2(0.6f, 1.0f),
			Vector2(0.75f, 1.0f),
			Vector2(1.0f, 1.0f),
			Vector2(1.85f, 1.0f),
	};
	Spline spline_colour_a = Spline(array_colour_a);
	psData_A->FillSplineColour(spline_colour_r, spline_colour_g, spline_colour_b, spline_colour_a);

	Vector2 array_velocity_linear_x[8] =
	{
			Vector2(-0.1f, 0.5f),
			Vector2(0.f, 0.0f),
			Vector2(0.2f, 0.0f),
			Vector2(0.55f, 0.0f),
			Vector2(0.6f, 0.0f),
			Vector2(0.75f, 0.0f),
			Vector2(1.0f, 0.0f),
			Vector2(1.85f, 0.7f),
	};
	Spline spline_velocity_linear_x = Spline(array_velocity_linear_x);

	Vector2 array_velocity_linear_y[8] =
	{
			Vector2(-0.1f, 1.0f),
			Vector2(0.f, 5.0f),
			Vector2(0.2f, 5.0f),
			Vector2(0.5f, 0.0f),
			Vector2(0.6f, 0.0f),
			Vector2(0.75f, -1.0f),
			Vector2(1.0f, -5.f),
			Vector2(1.85f, 0.7f),
	};
	Spline spline_velocity_linear_y = Spline(array_velocity_linear_y);

	Vector2 array_velocity_linear_z[8] =
	{
			Vector2(-0.1f, 0.0f),
			Vector2(0.f, 0.0f),
			Vector2(0.2f, 0.0f),
			Vector2(0.5f, 0.0f),
			Vector2(0.6f, 0.0f),
			Vector2(0.75f,0.0f),
			Vector2(1.0f, 0.f),
			Vector2(1.85f, 0.0f),
	};
	Spline spline_velocity_linear_z = Spline(array_velocity_linear_z);

	spline_velocity_linear_x *= 1.f;
	spline_velocity_linear_y *= 1.f;
	spline_velocity_linear_z *= 1.f;
	psData_A->FillSplineVelocityLinear(spline_velocity_linear_x, spline_velocity_linear_y, spline_velocity_linear_z);

	Vector2 array_velocity_orbital_x[8] =
	{
			Vector2(-0.1f, 0.5f),
			Vector2(0.f, 0.0f),
			Vector2(0.2f, 10.5f),
			Vector2(0.55f, -5.0f),
			Vector2(0.6f, -0.1f),
			Vector2(0.75f, 10.0f),
			Vector2(1.0f, 20.7f),
			Vector2(1.85f, 0.7f),
	};
	Spline spline_velocity_orbital_x = Spline(array_velocity_orbital_x);

	Vector2 array_velocity_orbital_y[8] =
	{
			Vector2(-0.1f, 0.5f),
			Vector2(0.f, 0.5f),
			Vector2(0.2f, 0.5f),
			Vector2(0.5f, 10.0f),
			Vector2(0.6f, 0.1f),
			Vector2(0.75f, 0.0f),
			Vector2(1.0f, -10.7f),
			Vector2(1.85f, 0.7f),
	};
	Spline spline_velocity_orbital_y = Spline(array_velocity_orbital_y);

	Vector2 array_velocity_orbital_z[8] =
	{
			Vector2(-0.1f, 0.5f),
			Vector2(0.f, 30.5f),
			Vector2(0.2f, 20.5f),
			Vector2(0.5f, 1.0f),
			Vector2(0.6f, -0.1f),
			Vector2(0.75f,-10.0f),
			Vector2(1.0f, 10.7f),
			Vector2(1.85f, -10.7f),
	};
	Spline spline_velocity_orbital_z = Spline(array_velocity_orbital_z);

	Vector2 array_velocity_orbital_r[8] =
	{
			Vector2(-0.1f, 0.5f),
			Vector2(0.f, 30.5f),
			Vector2(0.2f, 20.5f),
			Vector2(0.5f, 1.0f),
			Vector2(0.6f, -0.1f),
			Vector2(0.75f,-10.0f),
			Vector2(1.0f, 10.7f),
			Vector2(1.85f, -10.7f),
	};
	Spline spline_velocity_orbital_r = Spline(array_velocity_orbital_r);
	psData_A->FillSplineVelocityOrbital(spline_velocity_orbital_x, spline_velocity_orbital_y, spline_velocity_orbital_z, spline_velocity_orbital_r);

	Vector2 array_force_x[8] =
	{
			Vector2(-0.1f, 0.0f),
			Vector2(0.f, 0.0f),
			Vector2(0.2f, 0.0f),
			Vector2(0.55f, 0.0f),
			Vector2(0.6f, -0.0f),
			Vector2(0.75f, 0.0f),
			Vector2(1.0f, 0.0f),
			Vector2(1.85f, 0.0f),
	};
	Spline spline_force_x = Spline(array_force_x);
	
	Vector2 array_force_y[8] =
	{
			Vector2(-0.1f, 0.0f),
			Vector2(0.f, 0.0f),
			Vector2(0.2f, 0.0f),
			Vector2(0.5f, 0.0f),
			Vector2(0.6f, 0.0f),
			Vector2(0.75f, 0.0f),
			Vector2(1.0f, 0.0f),
			Vector2(1.85f, 0.7f),
	};
	Spline spline_force_y = Spline(array_force_y);

	Vector2 array_force_z[8] =
	{
			Vector2(-0.1f, 0.0f),
			Vector2(0.f, 0.5f),
			Vector2(0.2f, 0.5f),
			Vector2(0.5f, 0.0f),
			Vector2(0.6f, 0.0f),
			Vector2(0.75f, 0.0f),
			Vector2(1.0f, 0.0f),
			Vector2(1.85f, 0.0f),
	};
	Spline spline_force_z = Spline(array_force_z);
	spline_force_x *= 1.f;
	spline_force_y *= 1.f;
	spline_force_z *= 1.f;
	
	psData_A->FillSplineForce(spline_force_x, spline_force_y, spline_force_z);

	//std::cout << "----------psData_A----------" << std::endl << std::endl;
	//for (float t = 0.f; t <= 1.0f; t += 0.01f)
	//{
	//	std::cout << "spline_colour_r: " << spline_colour_r.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_colour_b: " << spline_colour_b.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_colour_g: " << spline_colour_g.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_colour_a: " << spline_colour_a.GetSplineValue(t) << std::endl;
	//
	//	std::cout << std::endl;
	//
	//	std::cout << "spline_velocity_linear_x: " << spline_velocity_linear_x.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_velocity_linear_y: " << spline_velocity_linear_y.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_velocity_linear_z: " << spline_velocity_linear_z.GetSplineValue(t) << std::endl;
	//
	//	std::cout << std::endl;
	//
	//	std::cout << "spline_force_x: " << spline_force_x.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_force_y: " << spline_force_y.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_force_z: " << spline_force_z.GetSplineValue(t) << std::endl;
	//
	//	std::cout << std::endl;
	//}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	psData_B = new ParticleSystemData(quad, 100000, 8.f, 100.0f, EmitterType::CUBE, Vector3(2, 2, 2), tex_depth_linear, tex_normal);
	psData_B->FillInitialForceAndVelocity(Vector3(), Vector3(50.f, 0.f, 1.f));
	
	array_colour_r[0] = Vector2(-0.1f, 0.0f);
	array_colour_r[1] = Vector2(0.f, 0.0f);
	array_colour_r[2] = Vector2(0.2f, 0.2f);
	array_colour_r[3] = Vector2(0.5f, 1.0f);
	array_colour_r[4] = Vector2(0.6f, 0.7f);
	array_colour_r[5] = Vector2(0.75f, 0.2f);
	array_colour_r[6] = Vector2(1.0f, 0.3f);
	array_colour_r[7] = Vector2(1.85f, 0.3f);
	
	spline_colour_r = Spline(0.7f);

	
	array_colour_g[0] = Vector2(-0.1f, 0.0f);
	array_colour_g[1] = Vector2(0.f, 1.0f);
	array_colour_g[2] = Vector2(0.2f, 0.8f);
	array_colour_g[3] = Vector2(0.5f, 0.2f);
	array_colour_g[4] = Vector2(0.6f, 0.7f);
	array_colour_g[5] = Vector2(0.75f, 0.5f);
	array_colour_g[6] = Vector2(1.0f, 0.6f);
	array_colour_g[7] = Vector2(1.85f, 0.3f);
	
	spline_colour_g = Spline(0.7f);

	
	array_colour_b[0] = Vector2(-0.1f, 0.5f);
	array_colour_b[1] = Vector2(0.f, 0.5f);
	array_colour_b[2] = Vector2(0.2f, 0.5f);
	array_colour_b[3] = Vector2(0.5f, 1.0f);
	array_colour_b[4] = Vector2(0.6f, 0.1f);
	array_colour_b[5] = Vector2(0.75f, 0.0f);
	array_colour_b[6] = Vector2(1.0f, 0.7f);
	array_colour_b[7] = Vector2(1.85f, 0.7f);
	
	spline_colour_b = Spline(0.7f);


	array_colour_a[0] = Vector2(-0.1f, 0.1f);	
	array_colour_a[1] = Vector2(0.f, 1.0f);		
	array_colour_a[2] = Vector2(0.2f, 1.0f);
	array_colour_a[3] = Vector2(0.5f, 1.0f);
	array_colour_a[4] = Vector2(0.6f, 1.0f);
	array_colour_a[5] = Vector2(0.75f, 0.5f);
	array_colour_a[6] = Vector2(1.0f, 0.0f);
	array_colour_a[7] = Vector2(1.85f, 0.0f);
													
	spline_colour_a = Spline(array_colour_a);
	psData_B->FillSplineColour(spline_colour_r, spline_colour_g, spline_colour_b, spline_colour_a);


	array_velocity_linear_x[0] = Vector2(-0.1f, 0.5f);
	array_velocity_linear_x[1] = Vector2(0.f, 0.0f);
	array_velocity_linear_x[2] = Vector2(0.2f, 0.0f);
	array_velocity_linear_x[3] = Vector2(0.55f, 0.0f);
	array_velocity_linear_x[4] = Vector2(0.6f, 0.0f);
	array_velocity_linear_x[5] = Vector2(0.75f, 0.0f);
	array_velocity_linear_x[6] = Vector2(1.0f, 0.0f);
	array_velocity_linear_x[7] = Vector2(1.85f, 0.7f);
	
	spline_velocity_linear_x = Spline(2.f);

	array_velocity_linear_y[0] = Vector2(-0.1f, 0.0f);
	array_velocity_linear_y[1] = Vector2(0.f, 0.0f);
	array_velocity_linear_y[2] = Vector2(0.1f, 0.0f);
	array_velocity_linear_y[3] = Vector2(0.3f, 0.3f);
	array_velocity_linear_y[4] = Vector2(0.5f, 1.0f);
	array_velocity_linear_y[5] = Vector2(0.75f, 1.0f);
	array_velocity_linear_y[6] = Vector2(1.0f, 1.f);
	array_velocity_linear_y[7] = Vector2(1.85f, 1.7f);
	
	spline_velocity_linear_y = Spline(array_velocity_linear_y);


	array_velocity_linear_z[0] = Vector2(-0.1f, 0.0f);
	array_velocity_linear_z[1] = Vector2(0.f, 0.0f);
	array_velocity_linear_z[2] = Vector2(0.2f, 0.0f);
	array_velocity_linear_z[3] = Vector2(0.5f, 0.0f);
	array_velocity_linear_z[4] = Vector2(0.6f, 0.0f);
	array_velocity_linear_z[5] = Vector2(0.75f, 0.0f);
	array_velocity_linear_z[6] = Vector2(1.0f, 0.f);
	array_velocity_linear_z[7] = Vector2(1.85f, 0.0f);
	
	spline_velocity_linear_z = Spline(0.f);

	spline_velocity_linear_x *= 1.f;
	spline_velocity_linear_y *= 1.f;
	spline_velocity_linear_z *= 1.f;
	psData_B->FillSplineVelocityLinear(spline_velocity_linear_x, spline_velocity_linear_y, spline_velocity_linear_z);

	
	array_velocity_orbital_x[0] = Vector2(-0.1f, 0.5f);
	array_velocity_orbital_x[1] = Vector2(0.f, 0.0f);
	array_velocity_orbital_x[2] = Vector2(0.2f, 10.5f);
	array_velocity_orbital_x[3] = Vector2(0.55f, -5.0f);
	array_velocity_orbital_x[4] = Vector2(0.6f, -0.1f);
	array_velocity_orbital_x[5] = Vector2(0.75f, 10.0f);
	array_velocity_orbital_x[6] = Vector2(1.0f, 20.7f);
	array_velocity_orbital_x[7] = Vector2(1.85f, 0.7f);
	
	spline_velocity_orbital_x = Spline(array_velocity_orbital_x);

	
	array_velocity_orbital_y[0] = Vector2(-0.1f, 0.5f);
	array_velocity_orbital_y[1] = Vector2(0.f, 0.5f);
	array_velocity_orbital_y[2] = Vector2(0.2f, 0.5f);
	array_velocity_orbital_y[3] = Vector2(0.5f, 10.0f);
	array_velocity_orbital_y[4] = Vector2(0.6f, 0.1f);
	array_velocity_orbital_y[5] = Vector2(0.75f, 0.0f);
	array_velocity_orbital_y[6] = Vector2(1.0f, -10.7f);
	array_velocity_orbital_y[7] = Vector2(1.85f, 0.7f);
	
	spline_velocity_orbital_y = Spline(array_velocity_orbital_y);

	
	array_velocity_orbital_z[0] = Vector2(-0.1f, 0.5f);
	array_velocity_orbital_z[1] = Vector2(0.f, 30.5f);
	array_velocity_orbital_z[2] = Vector2(0.2f, 20.5f);
	array_velocity_orbital_z[3] = Vector2(0.5f, 1.0f);
	array_velocity_orbital_z[4] = Vector2(0.6f, -0.1f);
	array_velocity_orbital_z[5] = Vector2(0.75f, -10.0f);
	array_velocity_orbital_z[6] = Vector2(1.0f, 10.7f);
	array_velocity_orbital_z[7] = Vector2(1.85f, -10.7f);
	
	spline_velocity_orbital_z = Spline(array_velocity_orbital_z);

	
	array_velocity_orbital_r[0] = Vector2(-0.1f, 0.5f);
	array_velocity_orbital_r[1] = Vector2(0.f, 30.5f);
	array_velocity_orbital_r[2] = Vector2(0.2f, 20.5f);
	array_velocity_orbital_r[3] = Vector2(0.5f, 1.0f);
	array_velocity_orbital_r[4] = Vector2(0.6f, -0.1f);
	array_velocity_orbital_r[5] = Vector2(0.75f, -10.0f);
	array_velocity_orbital_r[6] = Vector2(1.0f, 10.7f);
	array_velocity_orbital_r[7] = Vector2(1.85f, -10.7f);
	
	spline_velocity_orbital_r = Spline(array_velocity_orbital_r);
	psData_B->FillSplineVelocityOrbital(spline_velocity_orbital_x, spline_velocity_orbital_y, spline_velocity_orbital_z, spline_velocity_orbital_r);

	
	array_force_x[0] = Vector2(-0.1f, -1.0f);
	array_force_x[1] = Vector2(0.f, -1.0f);
	array_force_x[2] = Vector2(0.2f, -1.0f);
	array_force_x[3] = Vector2(0.55f, -1.0f);
	array_force_x[4] = Vector2(0.6f, -1.0f);
	array_force_x[5] = Vector2(0.75f, -1.0f);
	array_force_x[6] = Vector2(1.0f, -20.0f);
	array_force_x[7] = Vector2(1.85f, -20.0f);
	
	spline_force_x = Spline(array_force_x);

	
	array_force_y[0] = Vector2(-0.1f, 0.0f);
	array_force_y[1] = Vector2(0.f, 0.0f);
	array_force_y[2] = Vector2(0.2f, 0.0f);
	array_force_y[3] = Vector2(0.5f, 0.0f);
	array_force_y[4] = Vector2(0.6f, -2.0f);
	array_force_y[5] = Vector2(0.75f, -2.0f);
	array_force_y[6] = Vector2(1.0f, -2.0f);
	array_force_y[7] = Vector2(1.85f, -2.7f);
	
	spline_force_y = Spline(0.f);

	
	array_force_z[0] = Vector2(-0.1f, 0.0f);
	array_force_z[1] = Vector2(0.f, 0.0f);
	array_force_z[2] = Vector2(0.2f, 0.0f);
	array_force_z[3] = Vector2(0.5f, 0.0f);
	array_force_z[4] = Vector2(0.6f, 10.0f);
	array_force_z[5] = Vector2(0.75f, 10.0f);
	array_force_z[6] = Vector2(1.0f, 10.0f);
	array_force_z[7] = Vector2(1.85f, 10.0f);
	
	spline_force_z = Spline(array_force_z);
	spline_force_x *= 1.f;
	spline_force_y *= 1.f;
	spline_force_z *= 1.f;

	psData_B->FillSplineForce(spline_force_x, spline_force_y, spline_force_z);

	//std::cout << std::endl << "----------psData_B----------" << std::endl << std::endl;
	//for (float t = 0.f; t <= 1.0f; t += 0.01f)
	//{
	//	std::cout << "spline_colour_r: " << spline_colour_r.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_colour_b: " << spline_colour_b.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_colour_g: " << spline_colour_g.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_colour_a: " << spline_colour_a.GetSplineValue(t) << std::endl;
	//
	//	std::cout << std::endl;
	//
	//	std::cout << "spline_velocity_linear_x: " << spline_velocity_linear_x.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_velocity_linear_y: " << spline_velocity_linear_y.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_velocity_linear_z: " << spline_velocity_linear_z.GetSplineValue(t) << std::endl;
	//
	//	std::cout << std::endl;
	//
	//	std::cout << "spline_force_x: " << spline_force_x.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_force_y: " << spline_force_y.GetSplineValue(t) << std::endl;
	//	std::cout << "spline_force_z: " << spline_force_z.GetSplineValue(t) << std::endl;
	//
	//	std::cout << std::endl;
	//}


	particleSystem = new ParticleSystem(psData_B);


	if (!basicShader->LoadSuccess() || !OITShader->LoadSuccess() || !finalPassShader->LoadSuccess())
	{
		return;
	}



	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	init = true;
}
Renderer::~Renderer(void) 
{
	delete camera;
	delete triangle;
	delete quad;
	delete sphere;
	delete box;


	for (int i = 0; i < NUM_IN_GRID; i++)
		delete quads[i];

	delete[] quads;
	delete screenQuad;
	delete basicShader;
	delete OITShader;
	delete finalPassShader;
	//delete particleSystem;
	delete psData_A;
	delete psData_B;
	//delete basicComputeShader;
}

void Renderer::UpdateScene(float dt) 
{
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	viewMatrix = camera->BuildViewMatrix();
	modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(0.01f, 0.01f, 0.01f));
	camera->UpdateCamera(dt);
	particleSystem->Update(dt, modelMatrix, viewMatrix, projMatrix);

	//glUseProgram(basicComputeShader->GetProgram());
	//basicComputeShader->Dispatch(NUM_OF_INSTANCES, 1, 1);
	//glUseProgram(0);
}

void Renderer::RenderOITTestScene()
{
	glUseProgram(OITShader->GetProgram());
	 
	Vector4 glf = Vector4(1.f,1.f,1.f,1.f);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_OIT);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 1, (float*)&glf);
	glDepthMask(false);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	
	int i = 0;
	for (int x = 0; x < NUM_OF_X; x++)
	{
		for (int y = 0; y < NUM_OF_Y; y++)
		{
			for (int z = 0; z < NUM_OF_Z; z++)
			{
				Vector3 position = Vector3(x, y, -z) * 10.f;
				modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(1.0, 1.0, 1.0));
				glUniformMatrix4fv(glGetUniformLocation(OITShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
				glUniformMatrix4fv(glGetUniformLocation(OITShader->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);
				glUniformMatrix4fv(glGetUniformLocation(OITShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);
				quads[i]->Draw();
				i++;
			}
		}
	}
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	//glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );//
	
	glUseProgram(finalPassShader->GetProgram());
	
	glUniform1i(glGetUniformLocation(finalPassShader->GetProgram(), "tex_accumulation"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_accumulation);
	
	glUniform1i(glGetUniformLocation(finalPassShader->GetProgram(), "tex_reveal"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_reveal);
	
	screenQuad->Draw();
	
	glUseProgram(0);
	glDepthMask(true);



}

void Renderer::RenderOpaqueScene()
{
	Vector4 glf = Vector4(1.f, 1.f, 1.f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_opaque);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 2, (float*)&glf);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shader_opaque->GetProgram());
	//glUseProgram(basicShader->GetProgram());//
	glUniform1f(glGetUniformLocation(shader_opaque->GetProgram(), "near"), 1.0f);
	glUniform1f(glGetUniformLocation(shader_opaque->GetProgram(), "far"), 15000.0f);

	modelMatrix = Matrix4::Translation(Vector3(5, 0, 0)) * Matrix4::Rotation(90.f, Vector3(0, 1, 0)) * Matrix4::Scale(Vector3(20.0, 20.0, 20.0));
	glUniformMatrix4fv(glGetUniformLocation(shader_opaque->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(shader_opaque->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(shader_opaque->GetProgram(), "projMatrix"), 1, false, projMatrix.values);	
	//glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);//
	//glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);	 //
	//glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);	 //
	quad->Draw();
	modelMatrix = Matrix4::Translation(Vector3(0, 0, -250.f)) * Matrix4::Scale(Vector3(50.0, 50.0, 50.0));
	glUniformMatrix4fv(glGetUniformLocation(shader_opaque->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
	//glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);//
	quad->Draw();
	modelMatrix = Matrix4::Translation(Vector3(-100, 0, -1500)) * Matrix4::Scale(Vector3(10.0, 10.0, 10.0));
	glUniformMatrix4fv(glGetUniformLocation(shader_opaque->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
	//glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);//
	sphere->Draw();
	modelMatrix = Matrix4::Translation(Vector3(100, 0, -100)) * Matrix4::Scale(Vector3(20.0, 20.0, 20.0)) * Matrix4::Rotation(30.f, Vector3(1,1,1));
	glUniformMatrix4fv(glGetUniformLocation(shader_opaque->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
	//glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);//
	box->Draw();
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderNoise()
{
	glUseProgram(shader_noise->GetProgram());
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_noise);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniform2fv(glGetUniformLocation(shader_noise->GetProgram(), "u_resolution"), 1, (float*)&Vector2(256, 256));
	glUniform2fv(glGetUniformLocation(shader_noise->GetProgram(), "u_randomSeed"), 1, (float*)&Vector2(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
	//glUniform2fv(glGetUniformLocation(shader_noise->GetProgram(), "u_randomSeed"), 1, (float*)&Vector2(1, 1));
	glUniform1f(glGetUniformLocation(shader_noise->GetProgram(), "u_time"), std::time(0));
	quad->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

}

void Renderer::RenderScene() 
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	viewMatrix = camera->BuildViewMatrix();
	RenderOpaqueScene();
	modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(0.01f, 0.01f, 0.01f));
	particleSystem->Render(modelMatrix, viewMatrix, projMatrix);
	//RenderOITTestScene();
	//RenderNoise();
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA16;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::RecordFramerate(float dt)
{

}


