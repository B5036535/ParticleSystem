#include "Club.h"

Club::Club(Shader* s, Shader* shader_poster, Shader* shader_roof,GLuint tex, GLuint tex_poster)
{

	mesh = Mesh::LoadFromMeshFile("cube.msh");

	shader = s;
	texture = tex;

	SetModelScale(Vector3(100, 400, 100));

	boundingRadius = 10000.0f;

	poster = new Poster(shader_poster, tex_poster);
	AddChild(poster);
	poster->SetTransform(Matrix4::Translation(Vector3(0, 20, 80)));

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

Club::~Club()
{
}

void Club::Draw(const OGLRenderer& r)
{
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (mesh)
		mesh->Draw();
}

vector<LightNode*> Club::GetLightNodes()
{
	return lightNodes;
}

