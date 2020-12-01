#pragma once
#include "../nclgl/SceneNode.h"
#include "Poster.h"

class Club : public SceneNode
{
public:
	Club(Shader* shader, Shader* shader_poster, GLuint tex, GLuint tex_poster);
	~Club();

	void Draw(const OGLRenderer& r) override;
protected:
	Poster* poster;
};