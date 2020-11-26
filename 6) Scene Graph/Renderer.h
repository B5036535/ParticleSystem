#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/CubeRobot.h"
#include <nclgl\Camera.h>

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer();

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	void DrawNode(SceneNode* n);

	SceneNode* root;
	Camera* camera;
	Mesh* cube;
	Shader* shader;
};