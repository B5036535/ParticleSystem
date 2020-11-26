#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	meshes[0] = Mesh::GenerateTriangle();
	meshes[1] = Mesh::GenerateQuad();

	textures[0] = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	textures[1] = SOIL_load_OGL_texture(TEXTUREDIR"star.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!textures[0] || !textures[1])
		return;

	SetTextureRepeating(textures[0], true);
	SetTextureRepeating(textures[1], true);

	shader = new Shader(" TexturedVertex . glsl ", " StencilFragment . glsl ");

	if (!shader->LoadSuccess())
		return;

	usingScissor = false;
	usingStencil = false;
	init = true;
}

Renderer::~Renderer()
{
}

void Renderer::ToggleScissor()
{
}

void Renderer::ToggleStencil()
{
}
