#include "Bar.h"

Bar::Bar(Shader* s, Shader* shader_poster, Shader* shader_roof, GLuint tex)
{
	mesh = Mesh::LoadFromMeshFile("cube.msh");

	shader = s;
	texture = tex;

	SetModelScale(Vector3(100, 350, 100));

	boundingRadius = 10000.0f;

	wave = new Wave(shader_poster);
	AddChild(wave);
	wave->SetTransform(Matrix4::Translation(Vector3(-55, 150, 60)) * Matrix4::Rotation(90, Vector3(1, 0, 0)));

	roof = new Roof(shader_roof);
	AddChild(roof);
	roof->SetTransform(Matrix4::Translation(Vector3(0, 250, 0)));
	roof->SetModelScale(Vector3(20, 80, 20));

	tempLightNodes = roof->GetLightNodes();

	for (int i = 0; i < tempLightNodes.size(); i++)
	{
		lightNodes.push_back(tempLightNodes[i]);
	}

}

Bar::~Bar()
{
}

void Bar::Draw(const OGLRenderer& r)
{
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (mesh)
		mesh->Draw();
}

vector<LightNode*> Bar::GetLightNodes()
{
	return lightNodes;
}

//vector<Vector3> Bar::GetLightPositions()
//{
//	return lightPositions;
//}
//
//vector<Vector4> Bar::GetLightColours()
//{
//	return lightColours;
//}
//
//vector<float> Bar::GetLightRadii()
//{
//	return lightRadii;
//}
//