#include "Building.h"

Building::Building()
{
	mesh = Mesh::LoadFromMeshFile("cube.msh");
	modelScale = Vector3(50, 50, 50);
	tex_skyscraper_side = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraper.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_skyscraper_top = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraperRoof.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

}

Building::~Building()
{
	glDeleteTextures(1, &tex_skyscraper_side);
	glDeleteTextures(1, &tex_skyscraper_top);
}

void Building::Draw(const OGLRenderer& r)
{
		if (mesh)
		mesh->Draw();
}
