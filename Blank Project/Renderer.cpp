#include "Renderer.h"
#include <algorithm>
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0, 0, Vector3(0, 0, 10));

	triangle = Mesh::GenerateTriangle();
	quad = Mesh::GenerateQuad();
	screenQuad = Mesh::GenerateQuad();
	float offset = 10.f;
	int i = 0;
	for (int x = 0; x < NUM_OF_X; x++)
	{
		for (int y = 0; y < NUM_OF_Y; y++)
		{
			for (int z = 0; z < NUM_OF_Z; z++)
			{
				Vector4 colour = Vector4(std::min((float) x / (float)NUM_OF_X, 1.0f) , std::min((float)y / (float)NUM_OF_Y , 1.0f), 0.f,(float) z / (float) NUM_OF_Z);
				//Vector4 colour = Vector4((float)rand() / (float)RAND_MAX, (float) rand() / (float) RAND_MAX, (float) rand() / (float) RAND_MAX,(float) z / (float) NUM_OF_Z);
				//Vector4 colour = Vector4(1, 0, 0, 1);
				quads[i] = Mesh::GenerateQuad();
				quads[i]->UpdateColour(colour);
				i++;
			}
		}
	}

	basicShader = new Shader("vertex_basic.glsl", "fragment_basic.glsl");
	OITShader = new Shader("vertex_basicOIT.glsl", "fragment_OIT.glsl");
	finalPassShader = new Shader("vertex_OITFinalPass.glsl", "fragment_OITFinalPass.glsl");
	//basicComputeShader = new ComputeShader("compute_basic.glsl");

	//particleSystem = new ParticleSystem(3.f, 1.f,quad, EmitterType::SPHERE);

	//position = Vector3(0, 0, -200);

	if (!basicShader->LoadSuccess() || !OITShader->LoadSuccess() || !finalPassShader->LoadSuccess())
	{
		return;
	}


	GenerateScreenTexture(tex_accumulation);
	GenerateScreenTexture(tex_reveal);

	glGenFramebuffers(1, &FBO_OIT);
	GLenum OITbuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_OIT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_accumulation, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex_reveal, 0);
	glDrawBuffers(2, OITbuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	init = true;
}
Renderer::~Renderer(void) 
{
	delete camera;
	delete triangle;
	delete quad;
	delete[] quads;
	delete basicShader;
	delete OITShader;
	delete finalPassShader;
	//delete particleSystem;
	//delete basicComputeShader;
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	//particleSystem->Update(dt);
	//glUseProgram(basicComputeShader->GetProgram());
	//basicComputeShader->Dispatch(NUM_OF_INSTANCES, 1, 1);
	//glUseProgram(0);
}

void Renderer::RenderOITTestScene()
{
	glUseProgram(basicShader->GetProgram());
	modelMatrix = Matrix4::Translation(Vector3(10, 0, -250)) * Matrix4::Scale(Vector3(100.0, 100.0, 100.0));
	glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "viewMatrix"), 1, false, viewMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(basicShader->GetProgram(), "projMatrix"), 1, false, projMatrix.values);
	quad->Draw();
	glUseProgram(0);


	glUseProgram(OITShader->GetProgram());
	 
	GLfloat glf = 1.f;
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_OIT);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, tex_reveal, &glf);//
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

void Renderer::RenderScene() 
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	viewMatrix = camera->BuildViewMatrix();
	//modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(0.1, 0.1, 0.1));
	//particleSystem->Render(modelMatrix, viewMatrix, projMatrix);
	RenderOITTestScene();
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