#include "Roof.h"

Roof::Roof(Shader* s)
{
	mesh = Mesh::LoadFromMeshFile("cylinder.msh");
	shader = s;
	int numOfLights = 1;
}

Roof::~Roof()
{
}

void Roof::Draw(const OGLRenderer& r)
{
	if (mesh)
		mesh->Draw();
}

vector<Vector3> Roof::getLightPositions()
{
	return lightPositions;
}

vector<Vector4> Roof::getLightColours()
{
	return lightColours;
}
