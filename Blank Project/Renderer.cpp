#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	camera = new Camera();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	root = new SceneNode();
	poster = new Poster();
	building = new Building();

	building->SetTransform(Matrix4::Translation(Vector3(0, 0, -100)));
	poster->SetTransform(Matrix4::Translation(Vector3(0,0,-100)));
	//root->AddChild(poster);

	SetTextureRepeating(building->cubemap_skyscraper, true);

	shader_poster = new Shader("vertPoster.glsl", "fragPoster.glsl");
	shader_building = new Shader("vertBuilding.glsl", "fragBuilding.glsl");
	if(!shader_poster->LoadSuccess()) 
	{
		return;
	}

	glEnable(GL_BLEND);

	init = true;
}
Renderer::~Renderer(void)	
{
	delete camera;
	delete root;
	delete shader_poster;
	delete shader_building;
	delete building;
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(dt);
	building->Update(dt);
}

void Renderer::RenderScene()	
{
	glClearColor(0.2f,0.2f,0.2f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);	

	//DrawPoster();
	DrawBuilding();
	//DrawNode(root);
}

void Renderer::DrawPoster()
{
	BindShader(shader_poster);

	glUniform1i(glGetUniformLocation(shader_poster->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, poster->GetTexture());

	modelMatrix = poster->GetWorldTransform() * Matrix4::Scale(poster->GetModelScale());

	UpdateShaderMatrices();
}

void Renderer::DrawBuilding()
{
	BindShader(shader_building);

	glUniform1i(glGetUniformLocation(shader_building->GetProgram(), "cubeTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, building->cubemap_skyscraper);

	modelMatrix = building->GetWorldTransform() * Matrix4::Scale(building->GetModelScale());
	UpdateShaderMatrices();

	building->Draw(*this);
}

void Renderer::DrawNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		n->Draw(*this);
	}

	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i)
	{
		DrawNode(*i);
	}
}