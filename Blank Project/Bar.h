#pragma once
#include "../nclgl/SceneNode.h"
#include "Roof.h"
#include "Wave.h"

class Bar: public SceneNode
{
public:
	Bar(Shader* s, Shader* shader_poster, Shader* shader_roof, GLuint tex);
	~Bar();

	void Draw(const OGLRenderer& r) override;

	vector<LightNode*> GetLightNodes();


protected:
	Wave* wave;
	Roof* roof;

	int numOfLights;
	
	vector<LightNode*> lightNodes;
	vector<LightNode*> tempLightNodes;

};