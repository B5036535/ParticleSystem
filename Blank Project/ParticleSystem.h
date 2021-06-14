#pragma once

#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Matrix4.h"

class Mesh;
class Shader;
class ComputeShader;

enum class EmitterType
{
	RING = 1,
	CUBE = 2,
	SPHERE = 4,
	HEMISPHERE = 8,
	CONE = 16,
	POINT = 32,
};

struct Particle
{
	Vector4 colour;
	Vector4 position;
	Vector4 initvelocity;
	Vector4 velocity;
	Vector4 initforce;
	Vector4 force;
	Vector4 random;
};

class ParticleSystem
{
public:
	ParticleSystem(float time, float emissionTime, Mesh* mesh, EmitterType type);
	~ParticleSystem();
	
	Mesh*			mesh;

	void Update(float dt);
	void Render(Matrix4 model, Matrix4 view, Matrix4 projection);
private:

	const int	NUMBER_OF_INSTANCES = 10000;

	const float	MAX_LIFE_TIME;
	const float EMISSION_TIME;

	unsigned int	SSBO[2];
	bool			SSBOswitch;

	Shader*			shader_instance;
	ComputeShader*	shader_compute;

	EmitterType type;
	Vector3 emissionData;
	void Initialize();
};
