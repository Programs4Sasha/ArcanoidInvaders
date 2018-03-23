#pragma once
#include "cocos2d.h"

USING_NS_CC;

class EnemyNode : public cocos2d::Node
{

public:

	//� ������ ���� ����� ���������� calcBoundaries()
	float bottom;	// ������ �������
	float top;		// ������� �������
	float left;		// ����� �������
	float right;	// ������ ������� 
	float height;	// ������
	float width;	// ������

	void calcBoundaries();   // ���������� ������� ���� (all Children)
	Vec2 oldPosition;
	Vec2 speed_vector;

	//void setPosition(const Vec2& pos) override;
	//void setPosition(float x, float y) override;

	static EnemyNode* create();

	EnemyNode();
	~EnemyNode();

};

