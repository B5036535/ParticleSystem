#include "SceneNode.h"

SceneNode::SceneNode(Mesh* m, Vector4 c)
{
	mesh = m;
	colour = c;
	parent = NULL;
	modelScale = Vector3(1.f, 1.f, 1.f);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	texture = 0;
}

SceneNode::~SceneNode()
{
	for (int i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s)
{
	s->parent = this;
	children.push_back(s);
}

void SceneNode::Update(float dt)
{
	if (parent)
		worldTransform = parent->worldTransform * transform;
	else
		worldTransform = transform;

	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); i++)
	{
		(*i)->Update(dt);
	}
}

void SceneNode::Draw(const OGLRenderer& r)
{
	if (mesh)
		mesh->Draw();
}
