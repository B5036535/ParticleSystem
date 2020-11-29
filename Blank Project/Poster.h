#pragma once
#include "../nclgl/SceneNode.h"

class Poster : public SceneNode
{
public:
	Poster();
	~Poster();

	void Draw(const OGLRenderer& r) override;
};