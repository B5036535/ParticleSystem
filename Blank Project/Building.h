#pragma once
#include "../nclgl/SceneNode.h"

class Building : public SceneNode
{
public:
	Building();
	~Building();

	void Draw(const OGLRenderer& r) override;

	GLuint cubemap_skyscraper;
protected:
	
	GLuint tex_windows;
};