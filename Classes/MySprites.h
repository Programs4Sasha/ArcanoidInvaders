#pragma once
#include "cocos2d.h"

#define MAXX 8				// количество столбцов врагов
#define MAXY 3				//  количество рядов врагов   
#define ENEMY_RADIUS 12
#define MAXHEALTH 5
#define MAXSHIELD 3
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480
#define FORCEFIELDRADIUS 70
#define REPELLERS_VISIBLE false    //видимость репеллеров 
#define REPELLER_RADIUS 20
#define MULTIPLIER_INTERVAL 10.0f
#define MUSIC true


USING_NS_CC;

enum enemyType {enBasic, enForce, enDestoyer, enMultiplier};


class EnemySprite : public cocos2d::Sprite
{

public:
	virtual void decreaseHealth();
	enemyType type;
	bool attacking = false;
	int hp = 2; //@@

	int grid_x = 0; //индексы в массиве
	int grid_y = 0;
	Vec2 oldPosition;
	int width;
	int height;
	Size size;
	Vec2 speed_vector;
	float radius = ENEMY_RADIUS;

	void setPosition(const Vec2& pos) override;
	void setPosition(float x, float y) override;

	static EnemySprite* create(const std::string& filename);
	virtual void fire();
	virtual void attack();

	Sequence* createBulletSequence();

	EnemySprite();
	~EnemySprite();
};

class Enemy1 : public EnemySprite
{
public:
	static Enemy1* create();
	//void fire();
	void attack();
	Enemy1();

};

class BallSprite : public Sprite
{

public:
	Vec2 oldball_position;			// position of the ball from previous update
	static BallSprite* create();
	Vec2 vBall_speed;  //скорость шарика
	float raidus = 12;
	void explode();
	BallSprite();
	~BallSprite();
};

class Enemy2 : public EnemySprite
{
public:
	BallSprite* capturedBall;
	bool ball_captured = false;
	Sprite* force;
	static Enemy2* create();
	void fire() {};
	void attack();
	void deploy_force();
	Enemy2();
};

class Enemy3 : public EnemySprite
{
public:
	void decreaseHealth() override;
	static Enemy3* create();
	void fire() override;
	//Sequence* createBulletSequence(Sprite*& );
	void attack();
	Enemy3();
};

class Enemy4 : public EnemySprite
{
public:
	int createdEnemies = 0;
	static Enemy4* create();
	void fire();
	void attack();
	Enemy4();

};

class BulletSprite : public Sprite
{

public:

	static BulletSprite* create();
	Vec2 vBullet_speed;  //скорость шарика
	float radius = 13;


	BulletSprite();
	~BulletSprite();
};