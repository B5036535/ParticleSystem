#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Frustum.h"
#include "Poster.h"
#include "Building.h"
#include "CityBlock.h"

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
	Shader* shader_poster;
	Shader* shader_building;

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	GLuint tex_skyscraper_side;
	GLuint tex_skyscraper_top;

	void DrawPoster();
	void DrawBuilding();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();

	void GenerateScreenTexture(GLuint& into, bool depth = false);
};
