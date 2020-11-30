#pragma once
#include "../nclgl/SceneNode.h"

class Building : public SceneNode
{
public:
	Building();
	~Building();

	void Draw(const OGLRenderer& r) override;

	GLuint tex_skyscraper_side;
	GLuint tex_skyscraper_top;
protected:
	
	GLuint tex_windows;
};