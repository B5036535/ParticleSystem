#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/HeightMap.h"

class Wave : public SceneNode
{
public:
	Wave(Shader* shader, GLuint tex_noise);
	~Wave();

	void Draw(const OGLRenderer& r) override;
	void Update(float dt) override;

protected:
	float time;
};