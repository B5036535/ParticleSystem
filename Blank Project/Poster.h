#pragma once
#include "../nclgl/SceneNode.h"

class Poster : public SceneNode
{
public:
	Poster(Shader* shader, GLuint tex);
	~Poster();

	void Draw(const OGLRenderer& r) override;
};