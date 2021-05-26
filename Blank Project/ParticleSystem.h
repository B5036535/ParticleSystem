#pragma once

#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Matrix4.h"

class Mesh;
class Shader;
class ComputeShader;


struct Particle
{
	float	lifeTime;
	Vector4 colour;
	Vector3 position;
	Vector3 velocity;
	Vector3 force;
};

class ParticleSystem
{
public:
	ParticleSystem(float time, Mesh* mesh);
	~ParticleSystem();
	
	Mesh*			mesh;

	void Update(float dt);
	void Render(Matrix4 model, Matrix4 view, Matrix4 projection);
private:

	const int	NUMBER_OF_INSTANCES = 1;

	const float	MAX_LIFE_TIME;

	unsigned int	SSBO[2];
	bool			SSBOswitch;

	Shader*			shader_instance;
	ComputeShader*	shader_compute;



	void Initialize();
};
