#include "CityBlock.h"

CityBlock::CityBlock(Shader* s, GLuint tex_side, GLuint tex_top, int bpr)
{
	
	shader = s;
	num_of_buildings = bpr * bpr;
	for (int x = 0; x < bpr; x++)
	{
		for (int z = 0; z < bpr; z++)
		{
			building = new Building(shader, tex_side, tex_top);
			AddChild(building);
			building->SetTransform(Matrix4::Translation(Vector3(x, 0, z) * 22));
		}
	}
	hasShader = true;
}

CityBlock::~CityBlock()
{
}
