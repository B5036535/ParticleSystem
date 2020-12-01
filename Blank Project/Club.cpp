#include "Club.h"

Club::Club(Shader* s, Shader* shader_poster, GLuint tex, GLuint tex_poster)
{

	mesh = Mesh::LoadFromMeshFile("Cube.msh");
	shader = s;
	texture = tex;

	poster = new Poster(shader_poster, tex_poster);

	AddChild(poster);
}

Club::~Club()
{
}

void Club::Draw(const OGLRenderer& r)
{
}
