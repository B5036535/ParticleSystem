#include "Renderer.h"
#include <algorithm>
#include "../nclgl/Light.h"
const int LIGHT_NUM = 300;

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	
{
	camera = new Camera(-11, 0, Vector3(0, 200, 50));

	tex_skyscraper_side = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraper.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_skyscraper_top = SOIL_load_OGL_texture(TEXTUREDIR"SkyScraperRoof.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_poster = SOIL_load_OGL_texture(TEXTUREDIR"AnimePoster1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	tex_noise = SOIL_load_OGL_texture(TEXTUREDIR"perlin-noise-texture.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	SetTextureRepeating(tex_skyscraper_side, true);
	SetTextureRepeating(tex_skyscraper_top, true);


	root = new SceneNode();
	
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	quad_postProcess = Mesh::GenerateQuad();

	GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);


	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	shader_poster = new Shader("vertPoster.glsl", "fragPoster.glsl");
	shader_building = new Shader("vertBuilding.glsl", "fragBuilding.glsl");
	shader_club = new Shader("vertClub.glsl", "fragClub.glsl");
	shader_roof = new Shader("vertRoof.glsl", "fragRoof.glsl");
	shader_wave = new Shader("vertWave.glsl", "fragWave.glsl");
	shader_pointlight = new Shader("vertPointLight.glsl", "fragPointLight.glsl");
	shader_combine = new Shader("vertCombine.glsl", "fragCombine.glsl");
	

	if (!shader_poster->LoadSuccess() || !shader_building->LoadSuccess() || !shader_club->LoadSuccess() || !shader_roof->LoadSuccess() || !shader_wave->LoadSuccess() || !shader_pointlight->LoadSuccess() || !shader_combine->LoadSuccess())
	{
		return;
	}


	for (int x = 0; x < 7; x++)
	{
		for (int z = 0; z < 7; z++)
		{
			if (x == 1 && z == 1)
			{
				club = new Club(shader_club, shader_poster, shader_roof, tex_skyscraper_side, tex_poster);
				club->SetTransform(Matrix4::Translation(Vector3(x - 2.5f, 0, z - 2.5f) * (5 * 26) + Vector3(50, 140, 50)));;
				root->AddChild(club);
			}
			else if(x == 4 && z == 3)
			{
				bar = new Bar(shader_club, shader_wave, shader_roof, tex_skyscraper_side);
				bar->SetTransform(Matrix4::Translation(Vector3(x - 2.5f , 0, z - 2.5f) * (5 * 26) + Vector3(50, 140, 50)) * Matrix4::Rotation(-90, Vector3(0,1,0)));
				root->AddChild(bar);
			}
			else
			{
				block = new CityBlock(shader_building, tex_skyscraper_side, tex_skyscraper_top);
				block->SetTransform(Matrix4::Translation(Vector3(x - 2.5f, 0, z - 2.5f) * (5 * 26 + 5)));
				root->AddChild(block);
			}
		}
	}


	root->SetTransform(Matrix4::Translation(Vector3(-100, 0, -100)));

	root->Update(0.0f);
	pointLights = new Light[LIGHT_NUM];
	lightNodesClub = club->GetLightNodes();
	lightNodesBar = bar->GetLightNodes();


	for (int i = 0; i < LIGHT_NUM; i++)
	{
		Light& l = pointLights[i];

		if(i < lightNodesClub.size())
		{
			l.SetPosition(lightNodesClub[i]->GetWorldTransform().GetPositionVector());
			l.SetColour(lightNodesClub[i]->GetColour());
			l.SetRadius(lightNodesClub[i]->radius);
		}
		else if (i < lightNodesBar.size() + lightNodesClub.size() && i >= lightNodesClub.size())
		{
			l.SetPosition(lightNodesBar[i - lightNodesClub.size()]->GetWorldTransform().GetPositionVector());
			l.SetColour(lightNodesBar[i - lightNodesClub.size()]->GetColour());
			l.SetRadius(lightNodesBar[i - lightNodesClub.size()]->radius);
		}
		else
		{
			l.SetPosition(Vector3(rand() % 7, 20.0f, rand() % 7) * Vector3(26 * 5, 1, 26 * 5) + Vector3(-250, 0, -250));
			l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX), 0.5f + (float)(rand() / (float)RAND_MAX), 0.5f + (float)(rand() / (float)RAND_MAX), 1));
			l.SetRadius(20.0f + (rand() % 250));
		}
	}



	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
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
	delete shader_club;
	delete shader_roof;
	delete shader_wave;
	delete shader_pointlight;
	delete shader_combine;

	glDeleteTextures(1, &tex_poster);
	glDeleteTextures(1, &tex_skyscraper_side);
	glDeleteTextures(1, &tex_skyscraper_top);

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
}

void Renderer::UpdateScene(float dt) 
{
	camera->UpdateCamera(dt);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
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
	ClearNodeLists();
	DrawPointLights();
	CombineBuffers();
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
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

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



void Renderer::GenerateScreenTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}



void Renderer::DrawPointLights()
{
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(shader_pointlight);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(shader_pointlight->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(shader_pointlight->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(shader_pointlight->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(shader_pointlight->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invProjView = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(shader_pointlight->GetProgram(), "inverseProjView"), 1, false, invProjView.values);

	UpdateShaderMatrices();

	for (int i = 0; i < LIGHT_NUM; i++)
	{
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(shader_combine);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader_combine->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(shader_combine->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(shader_combine->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad_postProcess->Draw();
}

