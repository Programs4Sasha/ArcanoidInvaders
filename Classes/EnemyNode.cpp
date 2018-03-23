#include "EnemyNode.h"


EnemyNode::EnemyNode()
{
	setPosition(Vec2(50, 250));
}

EnemyNode::~EnemyNode()
{
}

void EnemyNode::calcBoundaries()
{
	Vector<Node*> Children = getChildren();
	Vector <Node*>::iterator ch;
	left = 1000;
	bottom = 1000;
	top = right = 0;


	for (ch=Children.begin(); ch != Children.end(); ++ch)
	{
		Node* child = *ch;

		if (left > child->getBoundingBox().getMinX())
			left = child->getBoundingBox().getMinX();
		
		if (right < child->getBoundingBox().getMaxX())
			right = child->getBoundingBox().getMaxX();

		if (top < child->getBoundingBox().getMaxY())
			top = child->getBoundingBox().getMaxY();

		if (bottom > child->getBoundingBox().getMinY())
			bottom = child->getBoundingBox().getMinY();
	}
	width = right - left; 
	height = top - bottom;
}

EnemyNode* EnemyNode::create()
{
	EnemyNode * ret = new (std::nothrow) EnemyNode();
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;

	for (int i = 0; i < 10; i++)
	{
		int j = 0;

		j++;
	}
}

