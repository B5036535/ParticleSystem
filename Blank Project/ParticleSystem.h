#pragma once

#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Matrix4.h"
#include "Spline.h"

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

struct ParticleSystemData
{
public:

	ParticleSystemData();


	ParticleSystemData(Mesh* m, int instances, float lTime, float eTime, EmitterType type, Vector3 eData);


	~ParticleSystemData()
	{
		delete[] dataFilled;
	}

	bool velocityWorldSpace;
	bool forceWorldSpace;

	Vector3		initialForce;
	Spline		force[3];

	Vector3		inititialVelocity;
	Spline		velocity_linear[3];
	Spline		velocity_orbital[4];

	Spline		colour[4];

	bool DataCheck();

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh();

	void SetNumberOfInstances(int n);
	int GetNumberOfInstances();

	void SetMaxLifeTime(float t);
	float GetMaxLifeTime();

	void SetEmitter(float time, EmitterType type, Vector3 d);
	float GetEmissionTime();
	EmitterType GetEmitterType();
	Vector3 GetEmissionData();

	void FillSplineForce(const Spline x, const Spline y, const Spline z);
	void FillSplineVelocityLinear(Spline x, Spline y, Spline z);
	void FillSplineVelocityOrbital(Spline x, Spline y, Spline z, Spline r);
	void FillSplineColour(Spline r, Spline g, Spline b, Spline a);
	void FillInitialForceAndVelocity(Vector3 f,Vector3 v);

	void GenerateNoise();

private:

	const int NUMBER_OF_DATA = 9;

	Mesh* mesh;
	int	NUMBER_OF_INSTANCES;
	float MAX_LIFE_TIME;
	float EMISSION_TIME;


	EmitterType emitter;
	Vector3		emissionData;
	bool* dataFilled;

	unsigned int FBO_noise;
	unsigned int tex_noise;
	
};

class ParticleSystem
{
public:
	//ParticleSystem(float time, float emissionTime, Mesh* mesh, EmitterType type);
	ParticleSystem(ParticleSystemData* data);
	~ParticleSystem();
	
	ParticleSystemData* data;

	Mesh*	mesh;
	Spline	forceSpline;
	Spline	colourSpline;

	void Update(float dt);
	void Render(Matrix4 model, Matrix4 view, Matrix4 projection);
private:
	float currentLifeTime;

	//const int	NUMBER_OF_INSTANCES = 200000;
	//
	//const float	MAX_LIFE_TIME;
	//const float EMISSION_TIME;

	unsigned int	SSBO[2];
	bool			SSBOswitch;

	unsigned int FBO_OIT;
	unsigned int FBO_collision;
	unsigned int tex_depth;
	unsigned int tex_colour;
	unsigned int buffer_accumulation;
	unsigned int buffer_reveal;

	Shader*			shader_instance;
	ComputeShader*	shader_compute;

	EmitterType type;
	Vector3 emissionData;
	void Initialize();
};
