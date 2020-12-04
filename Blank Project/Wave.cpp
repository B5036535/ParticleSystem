#include "Wave.h"

Wave::Wave(Shader* s)
{
	mesh = new HeightMap(TEXTUREDIR"noise.png");
	colour = Vector4(1, 1, 1, 0.5f);
	shader = s;
	time = 0.0f;

	modelScale = Vector3(0.03f, 3, 0.05f);
	boundingRadius = 200.0f;
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

	if (parent)
		worldTransform = parent->GetWorldTransform() * transform;
	else
		worldTransform = transform;

	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); i++)
	{
		(*i)->Update(dt);
	}
}
