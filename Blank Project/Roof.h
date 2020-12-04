#pragma once
#include "../nclgl/SceneNode.h"
#include "LightNode.h"
#include <vector>

class Roof : public SceneNode
{
public:
	Roof(Shader* shader);
	~Roof();

	void Draw(const OGLRenderer& r) override;

	vector<LightNode*> GetLightNodes();

	vector<Vector3> GetLightPositions();
	vector<Vector4> GetLightColours();
	vector<float> GetLightRadii();
protected:
	LightNode* lightNode;
	vector<LightNode*> lightNodes;

	int numOfLights;
	vector<Vector3> lightPositions;
	vector<Vector4> lightColours;
	vector<float> lightRadii;
};