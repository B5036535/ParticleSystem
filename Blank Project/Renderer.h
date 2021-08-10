#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Frustum.h"
//#include "../nclgl/ComputeShader.h"

#include "ParticleSystem.h"
class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene()				override;
	void UpdateScene(float msec)	override;

	void RenderOITTestScene();
	void RenderOpaqueScene();
	void RenderNoise();
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	
protected:

	//const static int NUM_OF_X = 4;
	//const static int NUM_OF_Y = 4;
	//const static int NUM_OF_Z = 4;
	//const static int NUM_IN_GRID = NUM_OF_X * NUM_OF_Y * NUM_OF_Z;

	const static int NUM_OF_INSTANCES = 100000;
	Camera* camera;
	
	ParticleSystemData* psData_A;
	ParticleSystemData* psData_B;

	ParticleSystem* particleSystem;

	Mesh* triangle;
	Mesh* quad;
	Mesh* sphere;
	Mesh* box;
	Mesh* screenQuad;
	//Mesh* quads[NUM_IN_GRID];
	Vector3 position;
	Vector2 offsets[NUM_OF_INSTANCES];

	Shader* basicShader;
	Shader* shader_opaque;
	Shader* OITShader;
	Shader* finalPassShader;
	Shader* shader_noise;

	GLuint FBO_opaque;
	GLuint tex_opaque;
	GLuint tex_depth_linear;
	GLuint tex_depth_exponential;
	GLuint tex_normal;
	
	GLuint FBO_OIT;
	GLuint tex_accumulation;
	GLuint tex_reveal;

	unsigned int FBO_noise;
	unsigned int tex_noise;

	ComputeShader* basicComputeShader;

	GLuint SSBO_Test;

};
