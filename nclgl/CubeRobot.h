#pragma once
#include "../nclgl/SceneNode.h"

class CubeRobot : public SceneNode
{
public:
	CubeRobot(Mesh* m);
	~CubeRobot() {};
	void Update(float dt) override;

protected:
	SceneNode* head;
	SceneNode* leftArm;
	SceneNode* rightArm;
};
