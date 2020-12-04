#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Frustum.h"
#include "Poster.h"
#include "Building.h"
#include "CityBlock.h"
#include "Wave.h"
#include "Club.h"
#include "Bar.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;
protected:
	Camera* camera;
	SceneNode* root;

	Building* building;
	Poster* poster;
	CityBlock* block;
	Wave* wave;
	Club* club;
	Bar* bar;


	vector<LightNode*> lightNodesBar;
	vector<LightNode*> lightNodesClub;

	Light* pointLights;
	Mesh* sphere;

	Mesh* quad_postProcess;
	
	Shader* shader_poster;
	Shader* shader_building;
	Shader* shader_club;
	Shader* shader_roof;
	Shader* shader_wave;
	Shader* shader_pointlight;
	Shader* shader_combine;
	
	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	GLuint tex_skyscraper_side;
	GLuint tex_skyscraper_top;
	GLuint tex_poster;
	GLuint tex_noise;

	GLuint heightTexture;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex;
	GLuint bufferDepthTex;
	GLuint bufferNormalTex;
	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	void PresentScene();
	void DrawPostProcess();

	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();

	void GenerateScreenTexture(GLuint& into, bool depth = false);
};
