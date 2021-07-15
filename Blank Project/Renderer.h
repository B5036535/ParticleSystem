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
	void GenerateScreenTexture(GLuint& into, bool depth = false);
protected:

	const static int NUM_OF_X = 4;
	const static int NUM_OF_Y = 4;
	const static int NUM_OF_Z = 4;
	const static int NUM_IN_GRID = NUM_OF_X * NUM_OF_Y * NUM_OF_Z;
	Camera* camera;
	
	ParticleSystem* particleSystem;

	Mesh* triangle;
	Mesh* quad;
	Mesh* screenQuad;
	Mesh* quads[NUM_IN_GRID];
	Vector3 position;
	//Vector2 offsets[NUM_OF_INSTANCES];

	Shader* basicShader;
	Shader* OITShader;
	Shader* finalPassShader;

	
	GLuint FBO_OIT;
	GLuint buffer_accumulation;
	GLuint tex_accumulation;
	GLuint buffer_reveal;
	GLuint tex_reveal;

	//ComputeShader* basicComputeShader;


	GLuint SSBO_Test;
};
