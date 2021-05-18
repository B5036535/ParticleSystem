#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Frustum.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene()				override;
	void UpdateScene(float msec)	override;
protected:
	Camera* camera;
	Mesh* triangle;
	Vector3 position;
	Vector2 offsets[100];
	Shader* basicShader;


	GLuint instanceVBO;
};
