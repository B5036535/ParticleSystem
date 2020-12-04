#include "LightNode.h"

LightNode::LightNode(Vector4 c, float r)
{
	colour = c;
	radius = r;
	modelScale = Vector3(1, 1, 1);

}

void LightNode::UpdateWorldTransform()
{
	if (parent)
		worldTransform = parent->GetWorldTransform() * transform;
	else
		worldTransform = transform;
}
