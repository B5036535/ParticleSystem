#include "Building.h"


Building::Building(Shader* s, GLuint tex_side, GLuint tex_top)
{
	shader = s;
	mesh = Mesh::LoadFromMeshFile("cube.msh");

	int biggestScale = (rand() % 50) + 50;

	modelScale = Vector3(20, biggestScale, 20);
	boundingRadius = biggestScale;

	tex_skyscraper_side = tex_side;
	tex_skyscraper_top = tex_top;

	offsetX = rand() % NUM_OF_WINDOWS_PER_ROW;
	offsetY= rand() % NUM_OF_WINDOWS_PER_ROW;
	ratio = 1 / NUM_OF_WINDOWS_PER_ROW;
}

Building::~Building()
{
	glDeleteTextures(1, &tex_skyscraper_side);
	glDeleteTextures(1, &tex_skyscraper_top);
}

void Building::Draw(const OGLRenderer& r)
{
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "sideTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "topTex"), 1);
	glUniform2f(glGetUniformLocation(shader->GetProgram(), "offset"), offsetX * ratio, offsetY * ratio);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_skyscraper_side);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_skyscraper_top);

	if (mesh)
		mesh->Draw();
}
