#pragma once
#include "cocos2d.h"

USING_NS_CC;

class EnemyNode : public cocos2d::Node
{

public:

	//с учетом всех детей выисляются calcBoundaries()
	float bottom;	// нижняя граница
	float top;		// верхняя граница
	float left;		// левая граница
	float right;	// привая граница 
	float height;	// высота
	float width;	// ширина

	void calcBoundaries();   // рассчитать границы узла (all Children)
	Vec2 oldPosition;
	Vec2 speed_vector;

	//void setPosition(const Vec2& pos) override;
	//void setPosition(float x, float y) override;

	static EnemyNode* create();

	EnemyNode();
	~EnemyNode();

};

