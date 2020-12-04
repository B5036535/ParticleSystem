#include "Roof.h"

Roof::Roof(Shader* s)
{
	mesh = Mesh::LoadFromMeshFile("Cylinder.msh");
	shader = s;
	int numOfLights = 1;
	SetBoundingRadius(50.0f);

	lightNode = new LightNode(Vector4(1.0f, 0.0f, 0.0f, 1.0f), 300.0f);
	AddChild(lightNode);
	lightNode->SetTransform(Matrix4::Translation(Vector3(0, 50.0f, 0.0f)));
	lightNodes.push_back(lightNode);

}

Roof::~Roof()
{
}

void Roof::Draw(const OGLRenderer& r)
{
	if (mesh)
		mesh->Draw();
}

vector<LightNode*> Roof::GetLightNodes()
{
	return lightNodes;
}

vector<Vector3> Roof::GetLightPositions()
{	
	return lightPositions;
}

vector<Vector4> Roof::GetLightColours()
{
	return lightColours;
}

vector<float> Roof::GetLightRadii()
{
	return lightRadii;
}
