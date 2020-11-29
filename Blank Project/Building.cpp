#include "Building.h"

Building::Building()
{
	mesh = Mesh::LoadFromMeshFile("cube.msh");
	modelScale = Vector3(20, 20, 20);
	//texture = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraper.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	cubemap_skyscraper = SOIL_load_OGL_cubemap(TEXTUREDIR"SkyScraper.jpg", TEXTUREDIR"SkyScraper.jpg", TEXTUREDIR"SkyScraperRoof.jpg", TEXTUREDIR"SkyScraperRoof.jpg", TEXTUREDIR"SkyScraper.jpg", TEXTUREDIR"SkyScraper.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!cubemap_skyscraper)
		std::cout << "no cube map" << std::endl;
}

Building::~Building()
{
}

void Building::Draw(const OGLRenderer& r)
{
		if (mesh)
		mesh->Draw();
}
