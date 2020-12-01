#include "Poster.h"

Poster::Poster(Shader* s, GLuint tex)
{
	mesh = Mesh::GenerateQuad();
	texture = tex;
	SetColour(Vector4(1, 1, 1, 0));
	modelScale = Vector3(100, 100, 1);
	shader = s;
	SetBoundingRadius(150.f);
}

Poster::~Poster()
{
}

void Poster::Draw(const OGLRenderer& r)
{
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (mesh)
		mesh->Draw();
}

