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

enum class MotionType
{
	CONSTANT,
	RANDOM_BETWEEN_TWO,
	SPLINE
};

enum class AppearanceType
{
	CONSTANT,
	TEXTURE,
	SPLINE
};

struct StageTypes
{
	EmitterType emission;
	MotionType motion;
	AppearanceType appearance;
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
	Vector4 collision;
};

struct ParticleSystemData
{
public:

	ParticleSystemData();


	ParticleSystemData(Mesh* m, int instances, float lTime, float eTime, EmitterType eType, Vector3 eData, MotionType mType, AppearanceType aType,unsigned int depthTex, unsigned int normalTex);


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

	Vector3 vec_force;
	Vector3 vec_force_max;
	Vector3 vec_linearVelocity;
	Vector3 vec_linearVelocity_max;

	Vector4 vec_colour;

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
	MotionType GetMotionType();
	AppearanceType GetAppearanceType();
	StageTypes GetTypes();

	Vector3 GetEmissionData();


	void ConstantMotion(Vector3 force, Vector3 linearVelocity);
	void RandomMotion(Vector3 forceMin, Vector3 forceMax, Vector3 linearVelocityMin, Vector3 linearVelocityMax);

	void ConstantAppearance(Vector4 colour);
	void TexturedAppearance(unsigned int texture);

	void FillSplineForce(const Spline x, const Spline y, const Spline z);
	void FillSplineVelocityLinear(Spline x, Spline y, Spline z);
	void FillSplineVelocityOrbital(Spline x, Spline y, Spline z, Spline r);
	void FillSplineColour(Spline r, Spline g, Spline b, Spline a);
	void FillInitialForceAndVelocity(Vector3 f,Vector3 v);

	void GenerateNoise();

	int GetTexAppearance() { return tex_appearance; }
	int GetTexDepth() { return tex_depth; };
	int GetTexNormal() { return tex_normals; };

private:

	const int NUMBER_OF_DATA = 9;

	Mesh* mesh;
	int	NUMBER_OF_INSTANCES;
	float MAX_LIFE_TIME;

	
	StageTypes types;

	float EMISSION_TIME;
	Vector3		emissionData;
	bool* dataFilled;



	unsigned int FBO_noise;
	unsigned int tex_noise;
	
	unsigned int tex_appearance;
	unsigned int tex_depth;
	unsigned int tex_normals;
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

	void Update(float dt, Matrix4 model, Matrix4 view, Matrix4 projection);
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
	unsigned int tex_colour;
	

	Shader*			shader_instance;
	ComputeShader*	shader_compute;

	EmitterType type;
	Vector3 emissionData;
	void Initialize();
};
