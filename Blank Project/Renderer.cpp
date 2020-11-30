#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	tex_skyscraper_side = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraper.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_skyscraper_top = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraperRoof.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	camera = new Camera();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	root = new SceneNode();
	poster = new Poster();
	

	poster->SetTransform(Matrix4::Translation(Vector3(0,0, -100)));
	//root->AddChild(poster);
	//root->AddChild(building);
	
	SetTextureRepeating(tex_skyscraper_side, true);
	SetTextureRepeating(tex_skyscraper_top, true);

	shader_poster = new Shader("vertPoster.glsl", "fragPoster.glsl");
	shader_building = new Shader("vertBuilding.glsl", "fragBuilding.glsl");


	block = new CityBlock(shader_building, tex_skyscraper_side, tex_skyscraper_top);
	root->AddChild(block);


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
	delete poster;
	delete shader_poster;
	delete shader_building;
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
	//DrawBuilding();
	//DrawPoster();
	ClearNodeLists();
}

void Renderer::DrawPoster()
{
	BindShader(shader_poster);

	glUniform1i(glGetUniformLocation(shader_poster->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, poster->GetTexture());

	modelMatrix = poster->GetWorldTransform() * Matrix4::Scale(poster->GetModelScale());

	UpdateShaderMatrices();

	poster->Draw(*this);
}

void Renderer::DrawBuilding()
{
	BindShader(shader_building);

	glUniform1i(glGetUniformLocation(shader_building->GetProgram(), "sideTex"), 0);
	glUniform1i(glGetUniformLocation(shader_building->GetProgram(), "topTex"), 1);
	glUniform2f(glGetUniformLocation(shader_building->GetProgram(), "offset"), building->offsetX * building->ratio, building->offsetY * building->ratio);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, building->tex_skyscraper_side);	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, building->tex_skyscraper_top);


	modelMatrix = building->GetWorldTransform() * Matrix4::Scale(building->GetModelScale());
	UpdateShaderMatrices();

	building->Draw(*this);
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
		Matrix4 modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		//std::cout << modelMatrix << std::endl;
		UpdateShaderMatrices();
		n->Draw(*this);
	}

}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}