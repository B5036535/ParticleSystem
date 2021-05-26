#include "ParticleSystem.h"
#include "../nclgl/Shader.h"
#include "../nclgl/ComputeShader.h"
#include "../nclgl/Mesh.h"

ParticleSystem::ParticleSystem(float time, Mesh* mesh)
	:
	SSBOswitch(true),
	MAX_LIFE_TIME(time)	
{
	this->mesh = mesh;
	shader_instance = new Shader("vertex_particle.glsl", "fragment_basic.glsl");
	shader_compute = new ComputeShader("compute_particle.glsl");

	if (!shader_instance->LoadSuccess()) {
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
	Particle* particles = new Particle[NUMBER_OF_INSTANCES];
	
	for (int i = 0; i < NUMBER_OF_INSTANCES; i++)
	{
		particles[i].lifeTime	= MAX_LIFE_TIME;
		particles[i].colour		= Vector4(1.0f, 1.0f, 1.0f, 1.f);
		particles[i].position	= Vector3(0.f,0.f,0.f);
		particles[i].velocity	= Vector3(0.f,0.f,0.f);
		particles[i].force		= Vector3(0.f,0.f,0.f);
	}


	glGenBuffers(2, SSBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUMBER_OF_INSTANCES * sizeof(Particle), particles, GL_STREAM_DRAW);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, SSBO[0], 0, sizeof(Particle) * NUMBER_OF_INSTANCES);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUMBER_OF_INSTANCES * sizeof(Particle), particles, GL_STREAM_DRAW);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, SSBO[1], 0, sizeof(Particle) * NUMBER_OF_INSTANCES);

	delete[] particles;
}

// Update particle SSBO i.e. Dispatch compute shader
void ParticleSystem::Update(float dt)
{
	glUseProgram(shader_compute->GetProgram());
	glUniform1i(glGetUniformLocation(shader_compute->GetProgram(), "SSBOswitch"), SSBOswitch);
	glUniform1f(glGetUniformLocation(shader_compute->GetProgram(), "dt"), dt);
	shader_compute->Dispatch(NUMBER_OF_INSTANCES, 1, 1);
	glUseProgram(0);
}

// Instance Render the mesh multiple times
void ParticleSystem::Render(Matrix4 model, Matrix4 view, Matrix4 projection)
{
	glUseProgram(shader_instance->GetProgram());
	glUniform1i(glGetUniformLocation(shader_instance->GetProgram(), "SSBOswitch"), SSBOswitch);
	glUniformMatrix4fv(glGetUniformLocation(shader_instance->GetProgram(), "modelMatrix"), 1, false, model.values);
	glUniformMatrix4fv(glGetUniformLocation(shader_instance->GetProgram(), "viewMatrix"), 1, false, view.values);
	glUniformMatrix4fv(glGetUniformLocation(shader_instance->GetProgram(), "projMatrix"), 1, false, projection.values);
	mesh->DrawInstance(NUMBER_OF_INSTANCES);
	glUseProgram(0);
	SSBOswitch = !SSBOswitch;
}
