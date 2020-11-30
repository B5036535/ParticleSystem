#pragma once
#include "../nclgl/SceneNode.h"

class Building : public SceneNode
{
public:
	Building(Shader* shader, GLuint tex_side, GLuint tex_top);
	~Building();

	void Draw(const OGLRenderer& r) override;

	GLuint tex_skyscraper_side;
	GLuint tex_skyscraper_top;

	int offsetX;
	int offsetY;

	const int NUM_OF_WINDOWS_PER_ROW = 32;
	float ratio;
protected:
	
};