#include "Poster.h"

Poster::Poster()
{
	mesh = Mesh::GenerateQuad();
	texture  = SOIL_load_OGL_texture(TEXTUREDIR"AnimePoster1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetColour(Vector4(1, 1, 1, 0));
	modelScale = Vector3(100, 100, 1);

}

Poster::~Poster()
{
}

void Poster::Draw(const OGLRenderer& r)
{
	if (mesh)
		mesh->Draw();
}

