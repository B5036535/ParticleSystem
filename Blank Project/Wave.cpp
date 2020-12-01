#include "Wave.h"

Wave::Wave(Shader* s, GLuint tex_noise)
{
	mesh = new HeightMap(TEXTUREDIR"noise.png");
	colour = Vector4(1, 1, 1, 0.5f);
	shader = s;
	time = 0.0f;
	texture = tex_noise;

	modelScale = Vector3(0.05f, 0.02f, 0.05f);
}

Wave::~Wave()
{
}

void Wave::Draw(const OGLRenderer& r)
{
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "time"), time);

	if (mesh)
		mesh->Draw();
}

void Wave::Update(float dt)
{
	time += dt;
}
