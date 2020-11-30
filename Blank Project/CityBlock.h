#pragma once
#include "Building.h"
#include "../nclgl/SceneNode.h"

class CityBlock : public SceneNode
{
public:
	CityBlock(Shader* shader, GLuint tex_side, GLuint tex_top, int buildings_per_row = 5);
	~CityBlock();

private:
	int num_of_buildings;
	Building* building;
};