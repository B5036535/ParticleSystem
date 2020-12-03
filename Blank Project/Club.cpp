#include "Club.h"

Club::Club(Shader* s, Shader* shader_poster, GLuint tex, GLuint tex_poster)
{

	mesh = Mesh::LoadFromMeshFile("cube.msh");

	shader = s;
	texture = tex;

	boundingRadius = 10000.0f;

	poster = new Poster(shader_poster, tex_poster);

	AddChild(poster);
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
