#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	camera = new Camera(-11, 0, Vector3(0, 200, 50));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	tex_skyscraper_side = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraper.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_skyscraper_top = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraperRoof.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_poster = SOIL_load_OGL_texture(TEXTUREDIR"AnimePoster1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_noise = SOIL_load_OGL_texture(TEXTUREDIR"perlin-noise-texture.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	SetTextureRepeating(tex_skyscraper_side, true);
	SetTextureRepeating(tex_skyscraper_top, true);


	root = new SceneNode();
	
	quad_postProcess = Mesh::GenerateQuad();

	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; ++i)
	{
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0])
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	shader_poster = new Shader("vertPoster.glsl", "fragPoster.glsl");
	shader_building = new Shader("vertBuilding.glsl", "fragBuilding.glsl");
	shader_wave = new Shader("vertWave.glsl", "fragWave.glsl");
	shader_postprocess = new Shader("vertTextured.glsl", "fragProcess.glsl");
	shader_scene = new Shader("vertTextured.glsl", "fragTextured.glsl");

	if (!shader_poster->LoadSuccess() || !shader_building->LoadSuccess() || !shader_wave->LoadSuccess() || !shader_postprocess->LoadSuccess() || !shader_scene->LoadSuccess())
	{
		return;
	}

	//wave = new Wave(shader_wave, tex_noise);
	//root->AddChild(wave);

	//for (int x = 0; x < 5; x++)
	//{
	//	for (int z = 0; z < 5; z++)
	//	{
	//		block = new CityBlock(shader_building, tex_skyscraper_side, tex_skyscraper_top);
	//		block->SetTransform(Matrix4::Translation(Vector3(x - 2.5f, 0, z - 5) * (5 * 26 + 5)));
	//		root->AddChild(block);
	//	}
	//}

	block = new CityBlock(shader_building, tex_skyscraper_side, tex_skyscraper_top);
	block->SetTransform(Matrix4::Translation(Vector3(-.5f, 0, -3) * (5 * 26 + 5)));
	root->AddChild(block);
	//poster = new Poster(shader_poster, tex_poster);
	//poster->SetTransform(Matrix4::Translation(Vector3(0, 0, -100)));
	//root->AddChild(poster);




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
	delete shader_wave;
	delete shader_postprocess;
	delete shader_scene;

	glDeleteTextures(1, &tex_poster);
	glDeleteTextures(1, &tex_skyscraper_side);
	glDeleteTextures(1, &tex_skyscraper_top);

	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
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

	DrawNodes();
	DrawPostProcess();
	PresentScene();
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
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (const auto& i : nodeList)
		DrawNode(i);

	for (const auto& i : transparentNodeList)
		DrawNode(i);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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


void Renderer::DrawPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(shader_postprocess);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shader_postprocess->GetProgram(), "sceneTex"), 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}


void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(shader_scene);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(shader_scene->GetProgram(), "diffuseTex"), 0);
	//quad_postProcess->Draw();
}



