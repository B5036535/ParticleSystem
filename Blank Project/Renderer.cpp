#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	tex_skyscraper_side = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraper.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_skyscraper_top = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraperRoof.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_poster = SOIL_load_OGL_texture(TEXTUREDIR"AnimePoster1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_noise = SOIL_load_OGL_texture(TEXTUREDIR"perlin-noise-texture.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	camera = new Camera(0,0,Vector3(0, 200, 50));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	root = new SceneNode();
	
	SetTextureRepeating(tex_skyscraper_side, true);
	SetTextureRepeating(tex_skyscraper_top, true);

	shader_poster = new Shader("vertPoster.glsl", "fragPoster.glsl");
	shader_building = new Shader("vertBuilding.glsl", "fragBuilding.glsl");
	shader_wave = new Shader("vertWave.glsl", "fragWave.glsl");

	//wave = new Wave(shader_wave, tex_noise);
	//root->AddChild(wave);

	for (int x = 0; x < 5; x++)
	{
		for (int z = 0; z < 5; z++)
		{
			block = new CityBlock(shader_building, tex_skyscraper_side, tex_skyscraper_top);
			block->SetTransform(Matrix4::Translation(Vector3(x, 0, z) * (5 * 26 + 5)));
			root->AddChild(block);
		}
	}



	//poster = new Poster(shader_poster, tex_poster);
	//poster->SetTransform(Matrix4::Translation(Vector3(0, 0, -100)));
	//root->AddChild(poster);


	if(!shader_poster->LoadSuccess() || !shader_building->LoadSuccess())
	{
		return;
	}

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	init = true;
}
Renderer::~Renderer(void)	
{
	delete camera;
	delete root;
	delete shader_poster;
	delete shader_building;
	glDeleteTextures(1, &tex_poster);
	glDeleteTextures(1, &tex_skyscraper_side);
	glDeleteTextures(1, &tex_skyscraper_top);
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
}

void Renderer::RenderScene()	
{
	BuildNodeLists(root);
	SortNodeLists();
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawNodes();
	ClearNodeLists();
}



void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f)
			transparentNodeList.push_back(from);
		else
			nodeList.push_back(from);
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); i++)
		BuildNodeLists((*i));
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes()
{
	for (const auto& i : nodeList)
		DrawNode(i);

	for (const auto& i : transparentNodeList)
		DrawNode(i);
}


void Renderer::DrawNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		BindShader(n->GetShader());
		modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		UpdateShaderMatrices();
		n->Draw(*this);
	}

}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}