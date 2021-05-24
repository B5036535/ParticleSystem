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
protected:

	const static int NUM_OF_COLS = 300;
	const static int NUM_OF_ROWS = 300;
	const static int NUM_OF_INSTANCES = NUM_OF_COLS * NUM_OF_ROWS;
	Camera* camera;
	
	ParticleSystem* particleSystem;

	Mesh* triangle;
	Mesh* quad;
	Vector3 position;
	Vector2 offsets[NUM_OF_INSTANCES];

	Shader* basicShader;
	//ComputeShader* basicComputeShader;


	GLuint SSBO_Test;
};
