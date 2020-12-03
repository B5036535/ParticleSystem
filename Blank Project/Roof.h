#pragma once
#include "../nclgl/SceneNode.h"
#include <vector>

class Roof : public SceneNode
{
public:
	Roof(Shader* shader);
	~Roof();

	void Draw(const OGLRenderer& r) override;

	vector<Vector3> getLightPositions();
	vector<Vector4> getLightColours();

protected:
	int numOfLights;
	vector<Vector3> lightPositions;
	vector<Vector4> lightColours;
};