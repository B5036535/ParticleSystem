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
	
	Shader* shader_skybox;
	Shader* shader_poster;
	Shader* shader_building;
	Shader* shader_club;
	Shader* shader_roof;
	Shader* shader_wave;
	Shader* shader_pointlight;
	Shader* shader_combine;
	Shader* shader_blur;
	Shader* shader_bloom;
	
	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	GLuint cubeMap;
	GLuint tex_skyscraper_side;
	GLuint tex_skyscraper_top;
	GLuint tex_poster;
	GLuint tex_noise;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferDepthTex;
	GLuint bufferNormalTex;

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;

	GLuint blurFBO;
	GLuint blurTex;

	GLuint bloomFBO;
	GLuint bloomColourTex;
	GLuint bloomBlurTex;

	void DrawCubeMap();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);


	void DrawPointLights();
	void CombineBuffers();
	void Blur();
	void Bloom();
	void GenerateScreenTexture(GLuint& into, bool depth = false);
};
