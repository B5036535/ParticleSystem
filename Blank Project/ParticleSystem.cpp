#include "ParticleSystem.h"
#include "../nclgl/Shader.h"
#include "../nclgl/ComputeShader.h"
#include "../nclgl/Mesh.h"

#include <ctime>
#include <iostream>
#include <algorithm>


ParticleSystemData::ParticleSystemData()
{
	dataFilled = new bool[NUMBER_OF_DATA];
	for (int i = 0; i < NUMBER_OF_DATA; i++)
		dataFilled[i] = false;


}


ParticleSystemData::ParticleSystemData(Mesh* m, int instances, float lTime, float eTime, EmitterType eType, Vector3 eData, MotionType mType, AppearanceType aType, unsigned int depthTex, unsigned int normalTex)
{
	dataFilled = new bool[NUMBER_OF_DATA];
	for (int i = 0; i < NUMBER_OF_DATA; i++)
		dataFilled[i] = false;

	SetMesh(m);
	SetNumberOfInstances(instances);
	SetMaxLifeTime(lTime);
	SetEmitter(eTime, eType, eData);


	tex_depth = depthTex;
	tex_normals = normalTex;
	//GenerateNoise();
}




bool ParticleSystemData::DataCheck()
{
	int i = 0;

	while (dataFilled[i] && i < NUMBER_OF_DATA)
		i++;

	return i == NUMBER_OF_DATA;
}

void ParticleSystemData::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;

	dataFilled[0] = true;
}
Mesh* ParticleSystemData::GetMesh()
{
	return mesh;
}

void ParticleSystemData::SetNumberOfInstances(int n)
{
	NUMBER_OF_INSTANCES = n;
	dataFilled[1] = true;
}
int ParticleSystemData::GetNumberOfInstances()
{
	return NUMBER_OF_INSTANCES;
}

void ParticleSystemData::SetMaxLifeTime(float t)
{
	MAX_LIFE_TIME = t;
	dataFilled[2] = true;
}
float ParticleSystemData::GetMaxLifeTime()
{
	return MAX_LIFE_TIME;
}

void ParticleSystemData::SetEmitter(float time, EmitterType type, Vector3 d)
{
	EMISSION_TIME = time;
	emitter = type;
	emissionData = d;
	dataFilled[3] = true;
}
float ParticleSystemData::GetEmissionTime()
{
	return EMISSION_TIME;
}
EmitterType ParticleSystemData::GetEmitterType()
{
	return emitter;
}
MotionType ParticleSystemData::GetMotionType()
{
	return motion;
}
AppearanceType ParticleSystemData::GetAppearanceType()
{
	return appearance;
}
Vector3 ParticleSystemData::GetEmissionData()
{
	return emissionData;
}

void ParticleSystemData::ConstantMotion(Vector3 force, Vector3 linearVelocity)
{
	if (motion != MotionType::CONSTANT)
		return;

	vec_force = force;
	vec_linearVelocity = linearVelocity;

	dataFilled[4] = true;
	dataFilled[5] = true;
	dataFilled[6] = true;
}

void ParticleSystemData::RandomMotion(Vector3 forceMin, Vector3 forceMax, Vector3 linearVelocityMin, Vector3 linearVelocityMax)
{
	if (motion != MotionType::RANDOM_BETWEEN_TWO)
		return;

	vec_force = forceMin;
	vec_force_max = forceMax;
	vec_linearVelocity = forceMin;
	vec_linearVelocity_max = forceMax;

	dataFilled[4] = true;
	dataFilled[5] = true;
	dataFilled[6] = true;
}

void ParticleSystemData::ConstantAppearance(Vector4 colour)
{
	if (appearance != AppearanceType::CONSTANT)
		return;

	vec_colour = colour;
	dataFilled[7] = true;
}

void ParticleSystemData::TexturedAppearance(unsigned int texture)
{
	if (appearance != AppearanceType::TEXTURE)
		return;

	tex_appearance = texture;

	dataFilled[7] = true;
}


void ParticleSystemData::FillSplineForce(const Spline x, const Spline y, const Spline z)
{
	if (motion != MotionType::SPLINE)
		return;

	for (int i = 0; i < 8; i++)
	{
		force[0].controlPoints[i].x = x.controlPoints[i].x;
		force[0].controlPoints[i].y = x.controlPoints[i].y;
		force[0].controlPoints[i].length_segment = x.controlPoints[i].length_segment;
		force[0].controlPoints[i].length_x = x.controlPoints[i].length_x;

		force[1].controlPoints[i].x = y.controlPoints[i].x;
		force[1].controlPoints[i].y = y.controlPoints[i].y;
		force[1].controlPoints[i].length_segment = y.controlPoints[i].length_segment;
		force[1].controlPoints[i].length_x = y.controlPoints[i].length_x;

		force[2].controlPoints[i].x = z.controlPoints[i].x;
		force[2].controlPoints[i].y = z.controlPoints[i].y;
		force[2].controlPoints[i].length_segment = z.controlPoints[i].length_segment;
		force[2].controlPoints[i].length_x = z.controlPoints[i].length_x;
	}

	dataFilled[4] = true;
}
void ParticleSystemData::FillSplineVelocityLinear(Spline x, Spline y, Spline z)
{
	if (motion != MotionType::SPLINE)
		return;

	for (int i = 0; i < 8; i++)
	{
		velocity_linear[0].controlPoints[i].x = x.controlPoints[i].x;
		velocity_linear[0].controlPoints[i].y = x.controlPoints[i].y;
		velocity_linear[0].controlPoints[i].length_segment = x.controlPoints[i].length_segment;
		velocity_linear[0].controlPoints[i].length_x = x.controlPoints[i].length_x;

		velocity_linear[1].controlPoints[i].x = y.controlPoints[i].x;
		velocity_linear[1].controlPoints[i].y = y.controlPoints[i].y;
		velocity_linear[1].controlPoints[i].length_segment = y.controlPoints[i].length_segment;
		velocity_linear[1].controlPoints[i].length_x = y.controlPoints[i].length_x;

		velocity_linear[2].controlPoints[i].x = z.controlPoints[i].x;
		velocity_linear[2].controlPoints[i].y = z.controlPoints[i].y;
		velocity_linear[2].controlPoints[i].length_segment = z.controlPoints[i].length_segment;
		velocity_linear[2].controlPoints[i].length_x = z.controlPoints[i].length_x;
	}

	dataFilled[5] = true;
}
void ParticleSystemData::FillSplineVelocityOrbital(Spline x, Spline y, Spline z, Spline r)
{

	if (motion != MotionType::SPLINE)
		return;

	for (int i = 0; i < 8; i++)
	{
		velocity_orbital[0].controlPoints[i].x = x.controlPoints[i].x;
		velocity_orbital[0].controlPoints[i].y = x.controlPoints[i].y;
		velocity_orbital[0].controlPoints[i].length_segment = x.controlPoints[i].length_segment;
		velocity_orbital[0].controlPoints[i].length_x = x.controlPoints[i].length_x;

		velocity_orbital[1].controlPoints[i].x = y.controlPoints[i].x;
		velocity_orbital[1].controlPoints[i].y = y.controlPoints[i].y;
		velocity_orbital[1].controlPoints[i].length_segment = y.controlPoints[i].length_segment;
		velocity_orbital[1].controlPoints[i].length_x = y.controlPoints[i].length_x;

		velocity_orbital[2].controlPoints[i].x = z.controlPoints[i].x;
		velocity_orbital[2].controlPoints[i].y = z.controlPoints[i].y;
		velocity_orbital[2].controlPoints[i].length_segment = z.controlPoints[i].length_segment;
		velocity_orbital[2].controlPoints[i].length_x = z.controlPoints[i].length_x;

		velocity_orbital[3].controlPoints[i].x = r.controlPoints[i].x;
		velocity_orbital[3].controlPoints[i].y = r.controlPoints[i].y;
		velocity_orbital[3].controlPoints[i].length_segment = r.controlPoints[i].length_segment;
		velocity_orbital[3].controlPoints[i].length_x = r.controlPoints[i].length_x;
	}

	dataFilled[6] = true;
}
void ParticleSystemData::FillSplineColour(Spline r, Spline g, Spline b, Spline a)
{
	if (appearance != AppearanceType::SPLINE)
		return;

	for (int i = 0; i < 8; i++)
	{
		colour[0].controlPoints[i].x = r.controlPoints[i].x;
		colour[0].controlPoints[i].y = r.controlPoints[i].y;
		colour[0].controlPoints[i].length_segment = r.controlPoints[i].length_segment;
		colour[0].controlPoints[i].length_x = r.controlPoints[i].length_x;

		colour[1].controlPoints[i].x = g.controlPoints[i].x;
		colour[1].controlPoints[i].y = g.controlPoints[i].y;
		colour[1].controlPoints[i].length_segment = g.controlPoints[i].length_segment;
		colour[1].controlPoints[i].length_x = g.controlPoints[i].length_x;

		colour[2].controlPoints[i].x = b.controlPoints[i].x;
		colour[2].controlPoints[i].y = b.controlPoints[i].y;
		colour[2].controlPoints[i].length_segment = b.controlPoints[i].length_segment;
		colour[2].controlPoints[i].length_x = b.controlPoints[i].length_x;

		colour[3].controlPoints[i].x = a.controlPoints[i].x;
		colour[3].controlPoints[i].y = a.controlPoints[i].y;
		colour[3].controlPoints[i].length_segment = a.controlPoints[i].length_segment;
		colour[3].controlPoints[i].length_x = a.controlPoints[i].length_x;
	}

	dataFilled[7] = true;
}

void ParticleSystemData::FillInitialForceAndVelocity(Vector3 f, Vector3 v)
{
	initialForce = f;
	inititialVelocity = v;


	dataFilled[8] = true;
}

void ParticleSystemData::GenerateNoise()
{
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

	Shader* shader_noise = new Shader("vertex_OITFinalPass.glsl", "fragment_noise.glsl");

	Mesh* quad = Mesh::GenerateQuad();

	glUseProgram(shader_noise->GetProgram());
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_noise);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniform2iv(glGetUniformLocation(shader_noise->GetProgram(), "u_resolution"), 1, (int*)&Vector2(256, 256));
	glUniform2fv(glGetUniformLocation(shader_noise->GetProgram(), "u_randomSeed"), 1, (float*)&Vector2(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
	glUniform1f(glGetUniformLocation(shader_noise->GetProgram(), "u_time"), std::time(0));
	quad->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	delete quad;
	delete shader_noise;
}

//ParticleSystem::ParticleSystem(float lifetime, float emissionTime,Mesh* mesh, EmitterType type)
//	:
//	SSBOswitch(true),
//	MAX_LIFE_TIME(lifetime),
//	EMISSION_TIME(emissionTime),
//	currentLifeTime(0.f)
//{
//	this->mesh = mesh;
//	this->type = type;
//
//	
//	emissionData = Vector3(10, 10, 10);
//
//	shader_instance = new Shader("vertex_particle.glsl", "fragment_basic.glsl");
//	shader_compute = new ComputeShader("compute_particle.glsl");
//
//	if (!shader_instance->LoadSuccess()) 
//	{
//		return;
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, FBO_OIT);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_ACCUM, GL_TEXTURE_2D, buffer_accumulation, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer_reveal, 0);
//
//
//	Initialize();
//}

ParticleSystem::ParticleSystem(ParticleSystemData* d)
{

	data = d;

	if (!data->DataCheck())
		return;


	shader_instance = new Shader("vertex_particle.glsl", "fragment_particle.glsl");
	shader_compute = new ComputeShader("compute_particle.glsl");
	
	//|| !shader_compute->LoadSuccess()
	if (!shader_instance->LoadSuccess() )
	{
		return;
	}




	Initialize();
}

ParticleSystem::~ParticleSystem()
{
	//delete mesh;
	delete shader_instance;
	delete shader_compute;
}

void ParticleSystem::Initialize()
{
	Particle* particles = new Particle[data->GetNumberOfInstances()];
	
	for (int i = 0; i < data->GetNumberOfInstances(); i++)
	{
		//particles[i].lifetime	= Vector4(MAX_LIFE_TIME, MAX_LIFE_TIME, EMISSION_TIME, (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * EMISSION_TIME);
		//particles[i].lifetime	= Vector4(MAX_LIFE_TIME, MAX_LIFE_TIME, EMISSION_TIME, 0);
		//std::cout << particles[i].lifetime.w << std::endl;
		particles[i].colour			= Vector4(1.0f, 0.0f, 1.f, 1.f);
		particles[i].position		= Vector4(0.f,0.f,0.f, 0.f);
		//particles[i].velocity		= Vector4(0.f,0.f,0.f, 0.f);
		particles[i].initvelocity	= Vector4(data->inititialVelocity, data->GetMaxLifeTime());
		particles[i].velocity		= Vector4(0.f, 0.f, 0.f, 0.f);
		particles[i].initforce		= Vector4(data->initialForce, data->GetEmissionTime());
		//particles[i].force			= Vector4(0.f,-5.f,0.f, 0);
		particles[i].force			= Vector4(0.f,0.f,0.f, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * data->GetEmissionTime());
		particles[i].random			= Vector4(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
		particles[i].collision		= Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	}


	glGenBuffers(2, SSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->GetNumberOfInstances() * sizeof(Particle), particles, GL_STREAM_DRAW);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, SSBO[0], 0, sizeof(Particle) * data->GetNumberOfInstances());


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->GetNumberOfInstances() * sizeof(Particle), particles, GL_STREAM_DRAW);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, SSBO[1], 0, sizeof(Particle) * data->GetNumberOfInstances());

	delete[] particles;
}

// Update particle SSBO i.e. Dispatch compute shader
void ParticleSystem::Update(float dt, Matrix4 model, Matrix4 view, Matrix4 projection)
{
	glUseProgram(shader_compute->GetProgram());
	glUniform1i(glGetUniformLocation(shader_compute->GetProgram(),	"SSBOswitch"), SSBOswitch);
	glUniform1f(glGetUniformLocation(shader_compute->GetProgram(),	"dt"), dt);
	//std::cout << std::time(0) << std::endl;
	glUniform1f(glGetUniformLocation(shader_compute->GetProgram(),	"time"), std::time(0));
	glUniform1i(glGetUniformLocation(shader_compute->GetProgram(),	"EmissionType"), (int)data->GetEmitterType());
	glUniform3fv(glGetUniformLocation(shader_compute->GetProgram(), "EmissionData"), 1, (float*)&data->GetEmissionData());

	if (data->GetMotionType() == MotionType::CONSTANT)
	{
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "vec_force"), 1, (float*) & (data->vec_force));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "vec_velocity_linear"), 1, (float*) & (data->vec_linearVelocity));
	}
	else if (data->GetMotionType() == MotionType::RANDOM_BETWEEN_TWO)
	{
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "vec_force"), 1, (float*) & (data->vec_force));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "vec_force_max"), 1, (float*) & (data->vec_force_max));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "vec_velocity_linear"), 1, (float*) & (data->vec_linearVelocity));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "vec_velocity_linear_max"), 1, (float*) & (data->vec_linearVelocity_max));
	}
	else
	{
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_force_x"), 8, (float*) & (data->force[0].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_force_y"), 8, (float*) & (data->force[1].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_force_z"), 8, (float*) & (data->force[2].controlPoints));

		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_velocity_linear_x"), 8, (float*) & (data->velocity_linear[0].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_velocity_linear_y"), 8, (float*) & (data->velocity_linear[1].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_velocity_linear_z"), 8, (float*) & (data->velocity_linear[2].controlPoints));

		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_velocity_orbital_x"), 8, (float*) & (data->velocity_orbital[0].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_velocity_orbital_y"), 8, (float*) & (data->velocity_orbital[1].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_velocity_orbital_z"), 8, (float*) & (data->velocity_orbital[2].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_velocity_orbital_r"), 8, (float*) & (data->velocity_orbital[3].controlPoints));
	}
	

	if (data->GetAppearanceType() == AppearanceType::CONSTANT)
	{
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "vec_colour"), 1, (float*) & (data->vec_colour));
	}
	else if (data->GetAppearanceType() == AppearanceType::SPLINE)
	{
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_colour_r"), 8, (float*) & (data->colour[0].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_colour_g"), 8, (float*) & (data->colour[1].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_colour_b"), 8, (float*) & (data->colour[2].controlPoints));
		glUniform4fv(glGetUniformLocation(shader_compute->GetProgram(), "spline_colour_a"), 8, (float*) & (data->colour[3].controlPoints));
	}

	glUniformMatrix4fv(glGetUniformLocation(shader_compute->GetProgram(), "modelMatrix"), 1, false, model.values);
	glUniformMatrix4fv(glGetUniformLocation(shader_compute->GetProgram(), "viewMatrix"), 1, false, view.values);
	glUniformMatrix4fv(glGetUniformLocation(shader_compute->GetProgram(), "projMatrix"), 1, false, projection.values);

	glUniform1f(glGetUniformLocation(shader_compute->GetProgram(), "near"), 1.0f);
	glUniform1f(glGetUniformLocation(shader_compute->GetProgram(), "far"), 15000.0f);


	glUniform1i(glGetUniformLocation(shader_compute->GetProgram(), "tex_depth"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data->GetTexDepth());
	glUniform1i(glGetUniformLocation(shader_compute->GetProgram(), "tex_normals"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, data->GetTexNormal());


	shader_compute->Dispatch(data->GetNumberOfInstances(), 1, 1);
	glUseProgram(0);
}

// Instance Render the mesh multiple times
// For OIT opaque surfaces needed to be rendered before this
void ParticleSystem::Render(Matrix4 model, Matrix4 view, Matrix4 projection)
{
	glUseProgram(shader_instance->GetProgram());
	glUniform1i(glGetUniformLocation(shader_instance->GetProgram(),			"SSBOswitch"), SSBOswitch);
	glUniformMatrix4fv(glGetUniformLocation(shader_instance->GetProgram(),	"modelMatrix"), 1, false, model.values);
	glUniformMatrix4fv(glGetUniformLocation(shader_instance->GetProgram(),	"viewMatrix"), 1, false, view.values);
	glUniformMatrix4fv(glGetUniformLocation(shader_instance->GetProgram(),	"projMatrix"), 1, false, projection.values);
	if (data->GetAppearanceType() == AppearanceType::TEXTURE)
	{
		glUniform1i(glGetUniformLocation(shader_compute->GetProgram(), "tex_display"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, data->GetTexAppearance());
	}
	data->GetMesh()->DrawInstance(data->GetNumberOfInstances());
	glUseProgram(0);
	SSBOswitch = !SSBOswitch;
}

