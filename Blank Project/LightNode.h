#pragma once
#include "../nclgl/SceneNode.h"

class LightNode : public SceneNode
{
public:
	LightNode(Vector4 colour, float radius);

	float radius;

	void UpdateWorldTransform();
};