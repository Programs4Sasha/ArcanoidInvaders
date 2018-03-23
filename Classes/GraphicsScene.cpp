#include "GraphicsScene.h"
#include <stdlib.h>
#include <math.h>
#include "MySprites.h"
#include "AudioEngine.h"

using namespace cocos2d::experimental;

//USING_NS_CC;

#include <iostream>
#include <sstream>

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}

GraphicsScene::~GraphicsScene()
{
	animation_enemy_expl->autorelease();
	animation_force_expand->autorelease();
	animation_shockwave->autorelease();
	animation_bullet->autorelease();
	paddle_explosion->autorelease();
	animation_bexpl->autorelease();
}

Scene* GraphicsScene::createScene()
{
	auto scene = Scene::create();
	auto layer = GraphicsScene::create();
	layer->setName("GraphicsScene");
	scene->addChild(layer);

	return scene;
}

bool GraphicsScene::init()
{
	if (AudioEngine::lazyInit())
	{
		CCLOG("Audio initialized successfully");
	}
	else
	{
		log("Error while initializing new audio engine");
	}

	bLeft = bRight = false;

	if (!Layer::init())
	{
		return false;
	}

	loadrepellers();

	auto background_sprite = Sprite::create("breakout_bg.png");
	background_sprite->setAnchorPoint(Vec2(0, 0));
	background_sprite->setPosition(0, 0);
	this->addChild(background_sprite, 0);

	shockwave_sprite = Sprite::create("shockwave\\shockwave_01.png");
	shockwave_sprite->setAnchorPoint(Vec2(0.5, 0.5));
	shockwave_sprite->setScale(1.0f);
	shockwave_sprite->setVisible(false);
	shockwave_sprite->setPosition(17, 12);
	this->addChild(shockwave_sprite, 0);
	//shockwave_sprite->retain(); //todo: release

	auto health_sprite = Sprite::create("heart.png");
	health_sprite->setAnchorPoint(Vec2(0, 0));
	health_sprite->setPosition(WINDOW_WIDTH - 90, 90);
	this->addChild(health_sprite, 0);

	auto shield_sprite = Sprite::create("shield.png");
	shield_sprite->setAnchorPoint(Vec2(0, 0));
	shield_sprite->setPosition(WINDOW_WIDTH-90, 40);
	this->addChild(shield_sprite, 0);

	paddle_sprite = Sprite::create("paddle.png");
	paddle_sprite->setAnchorPoint(Vec2(0, 1));
	paddle_sprite->setPosition(WINDOW_WIDTH/2, paddle_level);
	paddle_position = paddle_sprite->getPosition();
	background_sprite->addChild(paddle_sprite);

	BallSprite* ball_sprite = BallSprite::create();
	ball_sprite->setPosition(50, 50);
	vBalls.pushBack(ball_sprite);
	this->addChild(ball_sprite, 2);

	//ball_sprite = BallSprite::create();
	//ball_sprite->setPosition(600, 50);
	//ball_sprite->vBall_speed = Vec2(-50, 50);
	//vBalls.pushBack(ball_sprite);
	//this->addChild(ball_sprite, 2);

	//ball_sprite = BallSprite::create();
	//ball_sprite->setPosition(300, 400);
	//ball_sprite->vBall_speed = Vec2(-50, -100);
	//vBalls.pushBack(ball_sprite);
	//this->addChild(ball_sprite, 2);

	//ball_sprite = BallSprite::create();
	//ball_sprite->setPosition(360, 400);
	//ball_sprite->vBall_speed = Vec2(-100, -100);
	//vBalls.pushBack(ball_sprite);
	//this->addChild(ball_sprite, 2);	

	//ball_sprite = BallSprite::create();
	//ball_sprite->setPosition(300, 300);
	//ball_sprite->vBall_speed = Vec2(-50, -120);
	//vBalls.pushBack(ball_sprite);
	//this->addChild(ball_sprite, 2);	
	//
	//ball_sprite = BallSprite::create();
	//ball_sprite->setPosition(200, 400);
	//ball_sprite->vBall_speed = Vec2(-70, -100);
	//vBalls.pushBack(ball_sprite);
	//this->addChild(ball_sprite, 2);

	win_sprite = Sprite::create("win.png");
	win_sprite->setAnchorPoint(Vec2(0.5, 0.5));
	win_sprite->setPosition(WINDOW_WIDTH/2.0f,WINDOW_HEIGHT/2.0f);
	win_sprite->setScale(0.1f);
	win_sprite->setVisible(false);
	this->addChild(win_sprite, 0);

	try_sprite = Sprite::create("tryagain.png");
	try_sprite->setAnchorPoint(Vec2(0.5, 0.5));
	try_sprite->setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
	try_sprite->setScale(0.1f);
	try_sprite->setVisible(false);
	this->addChild(try_sprite, 3);

	MyEnemyNode = EnemyNode::create();
	MyEnemyNode->setName("MyEnemyNode");

	//CCString* name;
	char outString[50];
	std::string name;

	labelHealth = Label::createWithTTF ("10", "fonts\\font.ttf", 25);
	labelShield = Label::createWithTTF("5", "fonts\\font.ttf", 25);
	labelScore  = Label::createWithTTF("0", "fonts\\score.ttf", 25);

	labelHealth->setAnchorPoint(Vec2(0, 0));
	labelHealth->setPosition(Vec2(600, 90));
	this->addChild(labelHealth);

	labelShield->setAnchorPoint(Vec2(0, 0));
	labelShield->setPosition(Vec2(600, 40));
	this->addChild(labelShield);

	labelScore->setAnchorPoint(Vec2(0, 0));
	labelScore->setPosition(Vec2(15, WINDOW_HEIGHT - 50));
	labelScore->setColor(Color3B::YELLOW);
	labelScore->enableGlow(Color4B::YELLOW);
	this->addChild(labelScore);

	GenerateArmy();

	MyEnemyNode->setAnchorPoint(Vec2(0, 0));
	this->addChild(MyEnemyNode, 1);

	auto eventListener = EventListenerKeyboard::create();

	//move paddle by arrows
	
	bool left = false;
	bool right = false;

	eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {

		Vec2 loc = event->getCurrentTarget()->getPosition();
		Vector<Node*> myVector = MyEnemyNode->getChildren();
		Vector<Node*>::iterator myIterator;

		switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			case EventKeyboard::KeyCode::KEY_A:
				bLeft = true;
				break;
			case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			case EventKeyboard::KeyCode::KEY_D:
				bRight = true;
				break;
			case EventKeyboard::KeyCode::KEY_O:
				Krep += 1.0f;
				DBOUT("Krep =" << Krep << "\n");
				break;
			case EventKeyboard::KeyCode::KEY_L:
				Krep -= 1.0f;
				DBOUT("Krep =" << Krep << "\n");
				break;
			case EventKeyboard::KeyCode::KEY_I:
				Kenemy += 0.05f;
				DBOUT("Kenemy =" << Kenemy << "\n");
				break;

			case EventKeyboard::KeyCode::KEY_Y:
				if (attackingEnemy)
					attackingEnemy->decreaseHealth();
				break;
		}

	};
	
	eventListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {

		Vec2 loc = event->getCurrentTarget()->getPosition();
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			bLeft = false;
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			bRight = false;
			break;
		}
	};

	this->scheduleUpdate();
	this->schedule(schedule_selector(GraphicsScene::gameLogic), 4.0f);
	this->schedule(schedule_selector(GraphicsScene::shieldUp), 5.0f);
	this->schedule(schedule_selector(GraphicsScene::moveEnemies), 4.0f);
	this->schedule(schedule_selector(GraphicsScene::fireMultiplier), MULTIPLIER_INTERVAL);
	this->schedule(schedule_selector(GraphicsScene::fireall), 5.0f);

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, paddle_sprite);

	//create Animation object
	animation_enemy_expl = Animation::create();
	for (int i = 1; i <= 18; i++) 
	{
		sprintf(outString, "boom\\boom_%02d.png", i);
		name.assign(outString, 30);

		//name = CCString::createWithFormat("boom\\boom_%02d.png", i);
		animation_enemy_expl->addSpriteFrameWithFile(name);
	}
	animation_enemy_expl->setDelayPerUnit(float(0.5 / 19.0f));   //@implemented in createExplosionSeq()
	animation_enemy_expl->setRestoreOriginalFrame(false);
	animation_enemy_expl->retain(); 

	animation_bullet = Animation::create();
	for (int i = 1; i <= 12; i++)
	{
		//name = CCString::createWithFormat("rexpl\\rexpl_%02d.png", i);
		sprintf(outString, "rexpl\\rexpl_%02d.png", i);
		name.assign(outString, 30);
		animation_bullet->addSpriteFrameWithFile(name);
	}
	animation_bullet->setDelayPerUnit(float(0.5 / 12.0f));
	animation_bullet->setRestoreOriginalFrame(false);
	animation_bullet->retain(); 

	animation_force_expand = Animation::create();
	for (int i = 1; i <= 12; i++)
	{
		sprintf(outString, "force\\%02d.png", i);
		name.assign(outString, 30);
		animation_force_expand->addSpriteFrameWithFile(name);
	}
	animation_force_expand->setDelayPerUnit(float(2 / 25.0f));
	animation_force_expand->setRestoreOriginalFrame(false);
	animation_force_expand->retain(); 

	animation_paddle = Animation::create();
	for (int i = 1; i <= 27; i++)
	{
		sprintf(outString, "pexpl\\Pexpl%02d.png", i);
		name.assign(outString, 30);
		animation_paddle->addSpriteFrameWithFile(name);
	}
	animation_paddle->setDelayPerUnit(float(2 / 25.0f));
	animation_paddle->setRestoreOriginalFrame(true);
	paddle_explosion = Sequence::create(CCAnimate::create(animation_paddle),
		CallFuncN::create([&](Node* Sender) {GraphicsScene::paddleExplosionFinished(Sender); }), NULL);
	paddle_explosion->retain(); 

	animation_shockwave = Animation::create();
	for (int i = 1; i <= 30; i++)
	{
		sprintf(outString, "shockwave\\shockwave_%02d.png", i);
		name.assign(outString, 30);
		animation_shockwave->addSpriteFrameWithFile(name);
	}
	animation_shockwave->setDelayPerUnit(float(2 / 32.0f));
	animation_shockwave->setRestoreOriginalFrame(true);
	animation_shockwave->retain(); 

	animation_bexpl = Animation::create();
	for (int i = 1; i <= 14; i++)
	{
		sprintf(outString, "bexpl\\bexpl_%02d.png", i);
		name.assign(outString, 30);
		animation_bexpl->addSpriteFrameWithFile(name);
	}
	animation_bexpl->retain(); 

	MyEnemyNode->calcBoundaries();
	float Ntop	  = MyEnemyNode->top;
	float Nbottom = MyEnemyNode->bottom;
	float Nleft	  = MyEnemyNode->left;
	float Nright  = MyEnemyNode->right;
	float Nheight = MyEnemyNode->height;
	float Nwidth  = MyEnemyNode->width;

	float minx = 0;
	float maxx = WINDOW_WIDTH - MyEnemyNode->width;
	float miny = 200;
	float maxy = WINDOW_HEIGHT - MyEnemyNode->height;

	//auto action1 = MoveBy::create(3, Vec2(-50	, 50));
	//auto action2 = MoveBy::create(3, Vec2(100	,-40));
	//auto action3 = MoveBy::create(3, Vec2(-150	,10));
	//auto action4 = MoveBy::create(3, Vec2(50	,0));
	//auto action5 = MoveBy::create(3, Vec2(-50	,0));
	//auto action6 = MoveBy::create(3, Vec2(50	,0));
	//auto action7 = MoveBy::create(3, Vec2(-50	,0));
	//auto action8 = MoveBy::create(3, Vec2(50	,0));
	//Size S = MyEnemyNode->getContentSize();

	//MyEnemyNode->runAction(Sequence::create(action1, action2, action3, action4, action5, action6, action7, action8, 
	//	CallFuncN::create( [&](Node* Sender) {GraphicsScene::movingEnemiesFinished(Sender); }), NULL));

	return true;
}

void GraphicsScene::update(float delta) {

	//if (attackingEnemy)
	//{
	//	Vec2& speed = attackingEnemy->speed_vector;
	//	speed = speed.getLength()*0.99f*speed.getNormalized();
	//}

	Vector<EnemySprite*>::iterator myIterator;
	float curx = MyEnemyNode->getPosition().x;
	float oldx = MyEnemyNode->oldPosition.x;
	int margin;				//расстояние от места попадания до середины платформы
	Vec2 F;					//результирующая отталкивающая сила
	Vec2 dVa;				//приращение скорости атакующей цели
	double corr_right;		//поправка границы биты в зависимости от направления проэкции вектора скорости
	double corr_left;		//поправка границы биты в зависимости от направления проэкции вектора скорости
	double ball_hit_x;		//x-координата пересечения paddle_level
	double path_ratio;		//отношение пути до пересечения paddle_level к пройденному пути  
	double paddle_hit_x;	//координата x биты при пересечении paddle_level

	Vec2 curpos = MyEnemyNode->getPosition();
	Vec2 oldpos = MyEnemyNode->oldPosition;
	MyEnemyNode->speed_vector = (curpos - oldpos) / delta;

	char buffer[5];
	std::sprintf(buffer, "%d", health);
	labelHealth->setString(std::string(buffer));
	std::sprintf(buffer, "%d", shield);
	labelShield->setString(std::string(buffer));
	std::sprintf(buffer, "%2d", score);
	labelScore->setString(std::string(buffer));

	//подсчет скоростей целей в построении
	for (int j = 0; j < MAXY; j++)
	{
		for (int i = 0; i < MAXX; i++)
		{
			EnemySprite *spr = pEnemies[i][j];
			if (spr) {
				spr->oldPosition = MyEnemyNode->convertToWorldSpace(spr->getPosition());
				spr->speed_vector = MyEnemyNode->speed_vector;
			}
		}
	}

	MyEnemyNode->oldPosition = MyEnemyNode->getPosition();

	paddle_speed *= 1.0001;
	if (paddle_speed > 150) 
		paddle_speed = 150;

	//raidus = ball_sprite->getBoundingBox().size.width * 0.5f;
	paddle_width = paddle_sprite->getBoundingBox().size.width - 8; //shadow 8 pixels

	Vector <BallSprite*>::iterator curball;

	// обработка столкновений шариков со стенками и целями
	for (curball = vBalls.begin(); curball != vBalls.end(); ++curball)
	{

		BallSprite * ball_sprite = *curball;
		Vec2 ball_position = ball_sprite->getPosition();

		Vec2 &vBall_speed = ball_sprite->vBall_speed;
		Vec2 &oldball_position = ball_sprite->oldball_position;

		vBall_speed = vBall_speed * 1.0001f;

		float raidus = ball_sprite->raidus;
		if (ball_position.x <= raidus) {
			if (MUSIC)
				AudioEngine::play2d("hit.mp3", false, 0.05f);
			vBall_speed.x = -vBall_speed.x;
			ball_position.x = raidus;
		}
		if (ball_position.x >= WINDOW_WIDTH - raidus) {

			if (MUSIC)
				AudioEngine::play2d("hit.mp3", false, 0.1f);

			vBall_speed.x = -vBall_speed.x;
			ball_position.x = WINDOW_WIDTH - raidus;
		}
		if (ball_position.y >= WINDOW_HEIGHT - raidus) {
			if (MUSIC)
				AudioEngine::play2d("hit.mp3", false, 0.05f);
			ball_position.y = WINDOW_HEIGHT - raidus;
			vBall_speed.y = -vBall_speed.y;
		}
		//пересечение paddle_level
		if (ball_position.y < paddle_level + raidus)
		{
			//расчет координаты пересечения уровня биты (paddle_level)
			ball_hit_x = oldball_position.x - (oldball_position.x - ball_position.x)
				* (oldball_position.y - paddle_level - raidus) / (oldball_position.y - ball_position.y);

			//часть пути пройденного мячом до пересечения paddle_level
			if (oldball_position.x == ball_hit_x) {
				paddle_hit_x = paddle_position.x;
			}
			else
			{
				path_ratio = (oldball_position.x - ball_position.x) / (oldball_position.x - ball_hit_x);
				paddle_hit_x = oldpaddle_position.x + (paddle_position.x - oldpaddle_position.x)*path_ratio;
			}

			//попадание в биту
			Vec2 norm = vBall_speed;
			norm.normalize();

			if (norm.x < 0)
			{
				//шар летит влево
				corr_left = 5 * norm.x;
				corr_right = -10 * norm.x;;
			}
			else
			{
				//шар летит вправо
				corr_left = -10 * norm.x;
				corr_right = 5 * norm.x;

			}

			if (ball_hit_x > paddle_hit_x - raidus + corr_left && ball_hit_x < paddle_hit_x + paddle_width + raidus + corr_right) {

				if (MUSIC)
					AudioEngine::play2d("hit.mp3", false, 1.0f);

				vBall_speed.y = -vBall_speed.y;

				margin = ball_hit_x - (paddle_hit_x + paddle_width / 2);

				if (abs(margin) >= 60)
				{
					vBall_speed.rotate(Vec2(0, 0), -sgn(margin) * M_PI / 2);
				}
				else if (abs(margin) >= 56)
				{
					vBall_speed.rotate(Vec2(0, 0), -sgn(margin) * M_PI / 4);
				}
				else if (abs(margin) >= 48)
				{
					vBall_speed.rotate(Vec2(0, 0), -sgn(margin) * M_PI / 7);
				}
				else if (abs(margin) >= 32)
				{
					vBall_speed.rotate(Vec2(0, 0), -sgn(margin) * M_PI / 10);
				}

				//корректировка малых углов отскока, чтобы избежать отскока с движением шарика в 
				// почти коризонтальном направлении

				if (margin > 0) // отскок от правого края биты
				{
					if (vBall_speed.getAngle() < 0.1 * M_PI)
					{
						float angle;
						float angleshift;
						angle = vBall_speed.getAngle();
						angleshift = M_PI / 10 - vBall_speed.getAngle();
						vBall_speed.rotate(Vec2(0, 0), angleshift);
						angle = vBall_speed.getAngle();
					}
				}
				else // отскок от левого края биты
				{
					if (tohead(vBall_speed) < -2 * M_PI / 5) // -72 deg
					{
						float angleshift;
						angleshift = 2 * M_PI / 5 + tohead(vBall_speed);
						vBall_speed.rotate(Vec2(0, 0), angleshift);
					}

				}

				ball_position.y = paddle_level + raidus;
				//DBOUT("Hit the paddle ball_position.y = " << ball_position.y << "\n" 
				//	<< "margin" << margin << "\n"
				//	<< "paddle_position_x =" << paddle_position.x << "\n"
				//	<< "ball_position_x =" << ball_position.x << "\n" 
				//	<< "ball_hit_x =" << ball_hit_x << "\n");
			}
			else
			{
				//DBOUT("Missed paddle ball_position.y = " << ball_position.y << "\n"
				//	<< "paddle_position_x =" << paddle_position.x << "\n"
				//	<< "ball_position_x =" << ball_position.x << "\n"
				//	<< "ball_hit_x =" << ball_hit_x << "\n");
			}
		}

		ball_sprite->oldball_position = ball_position; // remember ball & paddle position 
		oldpaddle_position = paddle_position;
		ball_position += vBall_speed * delta;

		//шарик улетает через пол
		if (ball_position.y < -30) {
			if (vBalls.size() == 1) 
			{
				//ball_position.y = WINDOW_HEIGHT - 40 + vBall_speed.y * delta;
				ball_position = Vec2(10, 10);
				vBall_speed = Vec2(50, 50);
				decreaseHealth();
				explodePaddle();
			}
			else
			{
				vBallsToDelete.pushBack(ball_sprite);
			}
		}

		// проверяются столкновения со всеми врагами - элементами MyEnemyNode
		//mindist = pow(ENEMY_RADIUS + raidus, 2); //квадрат минимальной дистанции сближения

		for (myIterator = vAllSprites.begin(); myIterator != vAllSprites.end(); ++myIterator)
		{
			Node* enemyNode = *myIterator;
			EnemySprite* enSprite = (EnemySprite*)enemyNode;
			checkEnemyCollision(enSprite, ball_sprite, ball_position);
		}

		// проверка столкновения со всеми пулями 
		
		Vector <BulletSprite*>::iterator bull;
		for (bull = vBullets.begin(); bull != vBullets.end(); ++bull)
		{
			BulletSprite* curbullet=*bull;
			Vec2 dist = curbullet->getPosition() - ball_sprite->getPosition();
			if (dist.getLength() < curbullet->radius + ball_sprite->raidus) {

				ball_sprite->explode();
				
				if (vBalls.size() == 1)
				{
					ball_position = Vec2(10, 10);
					vBall_speed = Vec2(50, 50);
					decreaseHealth();
					explodePaddle();
				}
				else
				{
					vBallsToDelete.pushBack(ball_sprite);
				}
			}
		}

		ball_sprite->setPosition(ball_position);
		Vec2 norm = vBall_speed.getNormalized();

		if(ball_sprite->vBall_speed.getLength()>250)
			ball_sprite->vBall_speed = norm * 250;

	}

	//удаление улетевших шариков
	Vector<BallSprite*>::iterator itball;
	for (itball = vBallsToDelete.begin(); itball != vBallsToDelete.end(); ++itball)
	{
		this->removeChild(*itball);
		vBalls.eraseObject(*itball);
	}

	vBallsToDelete.clear();

	//попадание в Enemy2 - захват шарика
	Vec2 attEnemyPos;
	Vec2 dist;
	BallSprite* ballToDelete=NULL;
	
	if (attackingEnemy && attackingEnemy->attacking && attackingEnemy->type == enForce) {
		Enemy2* E2attEnemy = dynamic_cast <Enemy2*> (attackingEnemy);
		if (E2attEnemy) {
			for (itball = vBalls.begin(); itball != vBalls.end(); ++itball)
			{
				BallSprite *curball = *itball;
				
				dist = curball->getPosition() - attackingEnemy->getPosition();
				if (dist.getLength() < FORCEFIELDRADIUS && !E2attEnemy->ball_captured)
				{
					BallSprite* ball_sprite = BallSprite::create();
					ball_sprite->setPosition(attackingEnemy->convertToNodeSpace(curball->getPosition()));
					ball_sprite->setScale(2 / 3.0f);
					attackingEnemy->addChild(ball_sprite);
					ball_sprite->runAction(MoveTo::create(1, Vec2(0, 0)));
					E2attEnemy->ball_captured = true;
					E2attEnemy->capturedBall = ball_sprite;
					//ballToDelete = ball_sprite;
					if (vBalls.size() == 1)
					{
						decreaseHealth();
						explodePaddle();
						curball->setPosition(20, 20);
						curball->vBall_speed = Vec2(50, 50);
					}
					else
					{
						ballToDelete = curball;
					}
				}
			}

			if (ballToDelete)
			{
				this->removeChild(ballToDelete);
				vBalls.eraseObject(ballToDelete);
				//ballToDelete->retain(); //todo: ??? &release 
			}

		}
	}

	//attacking unit - расчет приращения скорости 
	//--------------
	//
	//расчет отталкивающей силы от всех целей построения и репеллеров периметра
	//
	float denom;
	if (attackingEnemy) {
		int maxforce = 1000;
		Vec2 distVector;
		Vec2 distVectorNorm;
		F = Vec2(0, 0); //результирующая сила
		Vector<EnemySprite*>::iterator itenemy;
		Vector<Sprite*>::iterator itrepeller;

		Vec2 Epos;
		for (itenemy = vAllSprites.begin(); itenemy != vAllSprites.end(); ++itenemy)
		{
			EnemySprite* E = *itenemy; // текущая цель из построения
			if (E == attackingEnemy)
				continue;

			Epos = MyEnemyNode->convertToWorldSpace(E->getPosition());
			distVector = attackingEnemy->getPosition() - Epos ;
							
			//distVector *= (distVector.getLength() - attackingEnemy->radius - E->radius) / distVector.getLength();
			distVectorNorm = distVector;
			distVectorNorm.normalize();
			Vec2 df;

			if (distVector.getLength() > E->radius * 4) {
				//df = Vec2 (0,0);
			}
			else if (distVector.getLengthSq() != 0) {
				denom = distVector.getLength();
				if (distVector.getLength() < (attackingEnemy->radius + E->radius))
					denom = 1;

				df = Kenemy * distVectorNorm / pow(denom,2);
				//DBOUT("df =" << df.getLength()  << "\n");
				if (df.getLength() > maxforce)
				{
					Vec2 dfnorm = df;
					dfnorm.normalize();
					df = dfnorm * maxforce;
				}

				//DBOUT(df.getLength() << "\n");
				F += df;
			}
		}
		
		for (itrepeller = vRepellers.begin(); itrepeller != vRepellers.end(); ++itrepeller)
		{
			Sprite* R = *itrepeller; // текущая цель из построения
			if (R == attackingEnemy)
				continue;
			distVector = attackingEnemy->getPosition() - R->getPosition();
			//distVector *= (distVector.getLength()/* - attackingEnemy->radius - 12*/) / distVector.getLength();
			distVectorNorm = distVector;
			distVectorNorm.normalize();
			if (distVector.getLengthSq() != 0) {
				denom = distVector.getLength();

				if (distVector.getLength() < (attackingEnemy->radius + 12))
					denom = 1;

				Vec2 df =Krep * distVectorNorm / pow(denom, 2);

				if (distVector.getLength() > REPELLER_RADIUS * 2) {
					//df = Vec2(0, 0);
				}
				if (df.getLength() > maxforce)
				{
					Vec2 dfnorm = df;
					dfnorm.normalize();
					df = dfnorm * maxforce;
				}
				F += df;

			}
		}

		//сила сопротивлени среды (вязкое трение)

		Vec2& speed= attackingEnemy->speed_vector;
		if (speed.getLength() > 100)
			F += -0.001*  pow(speed.getLength(), 2)  *speed.getNormalized();
		else
			F += -0.01*    speed.getLength()        *speed.getNormalized();

		dVa = F;

		attackingEnemy->speed_vector += dVa;

		//Vec2 &speed = attackingEnemy->speed_vector;
		//if (speed.getLength() > 100)
		//	speed = speed.getNormalized() * 100;

		attackingEnemy->setPosition(attackingEnemy->getPosition() + attackingEnemy->speed_vector * delta);

		if (attackingEnemy->getPosition().x > paddle_sprite->getPosition().x + paddle_width / 2.0f -10 &&
			attackingEnemy->getPosition().x < paddle_sprite->getPosition().x + paddle_width / 2.0f +10)
			attackingEnemy->speed_vector.x = 0;

		//столкновение атакующего с repellers_lower_level
		if (attackingEnemy->getPosition().y < repellers_lower_level + ENEMY_RADIUS)
			attackingEnemy->speed_vector.y = 0;

		//столкновение атакующего со стенками 
		if (attackingEnemy->getPosition().x < attackingEnemy->radius) {
			attackingEnemy->speed_vector.x = 0;
			attackingEnemy->setPosition(attackingEnemy->radius, attackingEnemy->getPosition().y);
		}
		if (attackingEnemy->getPosition().x > WINDOW_WIDTH - attackingEnemy->radius) {
			attackingEnemy->speed_vector.x = 0;
			attackingEnemy->setPosition(WINDOW_WIDTH - attackingEnemy->radius, attackingEnemy->getPosition().y);
		}

		if (attackingEnemy->getPosition().y > WINDOW_HEIGHT - attackingEnemy->radius) {
			attackingEnemy->speed_vector.y = 0;
			attackingEnemy->setPosition(attackingEnemy->getPosition().x, WINDOW_HEIGHT - attackingEnemy->radius);
		}

	}

	// end of attacking unit

	//позиция платформы
	if (bLeft)
		paddle_position.x -= paddle_speed * delta;
	else if (bRight)
		paddle_position.x += paddle_speed * delta;

	Vector<BulletSprite*>::iterator myi;
	Vector <BulletSprite*> bullets_to_delete;


	//*************************************************************************
	//обработка пуль
	for (myi = vBullets.begin(); myi != vBullets.end(); ++myi)
	{
		BulletSprite* bulletspr = *myi;

		if (bulletspr->getPosition().y < paddle_level && !explosion_in_progress)
		{
			if (bulletspr->getPosition().x > paddle_sprite->getPosition().x && bulletspr->getPosition().x < paddle_sprite->getPosition().x + paddle_width)
			{
				bullets_to_delete.pushBack(bulletspr);
				if (!explosion_in_progress) {
					shieldDown();
				}
			}
		}
	}

	for (myi = bullets_to_delete.begin(); myi != bullets_to_delete.end(); ++myi)
	{

		BulletSprite* spr;
		spr = *myi;
		spr->stopAllActions();
		spr->setPosition(Vec2(spr->getPosition().x, paddle_level + 1));

		auto bullet_explosion = Sequence::create(CCAnimate::create(animation_bullet),
			CallFuncN::create([&](Node* Sender) {GraphicsScene::bulletExplosionFinished(Sender); }),
			NULL);

		spr->runAction(bullet_explosion);
		//cleanup implemented in callback
		//this->removeChild(*myi);      
		//vBullets.eraseObject(*myi);
	}
	
	paddle_sprite->setPosition(paddle_position);

	Vec2 pos = MyEnemyNode->getPosition();
	if (pos.y < 0)
		MyEnemyNode->setPosition(Vec2(pos.x, 0));
	//MyEnemyNode ->runAction ( MoveBy::create(5, Vec2(0, 30)) );

	if (pos.y > WINDOW_HEIGHT - MyEnemyNode->height)
		MyEnemyNode->setPosition(Vec2(pos.x, WINDOW_HEIGHT - MyEnemyNode->height));

	if (paddle_position.x < 0)
	{
		paddle_position.x = 0;
	}
	if (paddle_position.x > WINDOW_WIDTH - paddle_width)
		paddle_position.x = WINDOW_WIDTH - paddle_width;

}
//update

Sequence * GraphicsScene::createExplosionSeq()
{
	if (!animation_enemy_expl)
	{
		//DBOUT("animation_enemy_expl = NULL\n");
		return NULL;
	}

	animation_enemy_expl->setDelayPerUnit(float(0.5 / 19.0f));
	animation_enemy_expl->setRestoreOriginalFrame(false);
	//Node* Sender = this->attackingEnemy; //todo: delete this
	Sequence *explosion = Sequence::create(CCAnimate::create(animation_enemy_expl),
		CallFuncN::create([&](Node* Sender) {GraphicsScene::enemyExplosionFinished(Sender); }),
		NULL);

	return explosion; 
}

Sequence * GraphicsScene::createBallExplosionSeq()
{
	if (!animation_bexpl)
	{
		//DBOUT("animation_enemy_expl = NULL\n");
		return NULL;
	}

	animation_bexpl->setDelayPerUnit(float(1 / 14.0f));
	animation_bexpl->setRestoreOriginalFrame(false);
	Sequence *explosion = Sequence::create(CCAnimate::create(animation_bexpl),
		CallFuncN::create([&](Node* Sender) {Sender->removeFromParent(); }),
		NULL);

	return explosion;
}

int GraphicsScene::sgn(double x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}

// возвращает heading angle (угол от направления вперед (+- Pi) плюс по часовой стрелке)
float GraphicsScene::tohead(Vec2 vector)
{
	float angle, heading;
	
	angle = vector.getAngle();

	if (angle > -M_PI / 2)
		heading = M_PI / 2 - angle;
	else
		heading = -3 * M_PI / 2 - angle;

	return heading;
}

//explosion Finished
void GraphicsScene::enemyExplosionFinished(Node* pSender)
{
	EnemySprite* enemy = (EnemySprite*) pSender;

	pEnemies[enemy->grid_x] [enemy->grid_y] = NULL;
	score++;

	if (enemy->attacking)
	{
		if (enemy->type == enForce)
		{
			//отпустить пойманный шарик
			Enemy2* enemy = dynamic_cast<Enemy2*> (pSender);
			if (enemy->ball_captured == true) {				
				auto ball_sprite = BallSprite::create();
				ball_sprite->setPosition(enemy->convertToWorldSpace(enemy->capturedBall->getPosition()));
				ball_sprite->vBall_speed = Vec2(50, 50);
				vBalls.pushBack(ball_sprite);
				this->addChild(ball_sprite, 2);
			}
		}
		
		vAllSprites.eraseObject(enemy);
		vSpritesToDelete.eraseObject(enemy);

		if (vAllSprites.size() == 0 ) 
		{
			auto scaleBy = ScaleTo::create(0.5f, 1.0f);
			win_sprite->setVisible(true);

			auto delayAction = DelayTime::create(3.0f);
			auto funcCallback = CallFunc::create([=]() {
				GraphicsScene::restart();
			});
			win_sprite->runAction(Sequence::create(scaleBy, delayAction, funcCallback, NULL));
		}

		// enemy3 - урон соседним врагам -1hp
		//цикл по всем  спрайтам
		//Vector<EnemySprite*>::iterator en;
		//сортировка масссива целей по расстоянию до attackingEnemy

		//std::sort(vAllSprites.begin(), vAllSprites.end(), [=](EnemySprite* a, EnemySprite* b) {
		//	float dist1 = (MyEnemyNode->convertToWorldSpace(a->getPosition()) - attackingEnemy->getPosition()).getLength();
		//	float dist2 = (MyEnemyNode->convertToWorldSpace(b->getPosition()) - attackingEnemy->getPosition()).getLength();
		//	return dist1 < dist2; //от меньшего к большему
		//});

		//if (enemy->type == enDestoyer)
		//{
		//	int count = 0;
		//	for (en = vAllSprites.begin(); en != vAllSprites.end(); ++en)
		//	{

		//		EnemySprite* curEnemy = *en;
		//		if (curEnemy == attackingEnemy) 
		//			continue;

		//		if (count < 10)
		//		{
		//			curEnemy->decreaseHealth();
		//			count++;
		//		}
		//	}
		//}

		this->removeChild(enemy);
		attack_in_progress = false;
		attackingEnemy->release();
		attackingEnemy = NULL;
	}
	else 
	{
		MyEnemyNode->removeChild(enemy);
		vAllSprites.eraseObject(enemy);
		vSpritesToDelete.eraseObject(enemy);
		if (vAllSprites.size() == 0) {
			auto scaleTo = ScaleTo::create(0.5f, 1.0f);
			win_sprite->setVisible(true);
			auto delayAction = DelayTime::create(3.0f);
			win_sprite->runAction(Sequence::create(scaleTo, delayAction, CallFunc::create([=]() { GraphicsScene::restart(); }), NULL));
		}
		enemy->release();
	}

	CCLOG("Animation Finished (enemy explosion)");
}

void GraphicsScene::tryagainFinished(Node * pSender)
{
	restart();
}

void GraphicsScene::fireMultiplier(float)
{
	if (attackingEnemy && attackingEnemy->type == enMultiplier)
		attackingEnemy -> fire();
}

void GraphicsScene::fireall(float)
{

	//for (int x = 1; x < MAXX; x++)
	//{
	//	if (pEnemies[x][0])
	//	{
	//		pEnemies[x][0]->fire();
	//		//break;
	//	}
	//	
	//}
}

void GraphicsScene::movingEnemiesFinished(Node* pSender)
{
	auto action1 = MoveBy::create(3, Vec2(-50, 50));
	auto action2 = MoveBy::create(3, Vec2(100, -40));
	auto action3 = MoveBy::create(3, Vec2(-150, 10));
	auto action4 = MoveBy::create(3, Vec2(50, 0));
	auto action5 = MoveBy::create(3, Vec2(-50, 0));
	auto action6 = MoveBy::create(3, Vec2(50, 0));
	auto action7 = MoveBy::create(3, Vec2(-50, 0));
	auto action8 = MoveBy::create(3, Vec2(50, 0));

	MyEnemyNode->runAction(Sequence::create(action1, action2, action3, action4, action5, action6, action7, action8,
		CallFuncN::create([&](Node* Sender) {GraphicsScene::movingEnemiesFinished(Sender); }), NULL));
}

//void GraphicsScene::bulletFinished(Node* pSender)
//{
//	this->removeChild(pSender);
//	vBullets.eraseObject((Sprite*)pSender);
//	CCLOG("Bullet flight finished");
//}

//void GraphicsScene::attackManeuverFinished(Node* pSender)
//{
//	auto action = MoveBy::create(0.5f,Vec2(0,rand()%20-10));
//	attackingEnemy->runAction(action);
//	attackingEnemy->fire();
//
//	CCLOG("Attack maneuver finished");
//}
void GraphicsScene::bulletExplosionFinished(Node* pSender)
{
	this->removeChild(pSender);
	vBullets.eraseObject((BulletSprite*)pSender);
	CCLOG("Bullet animation finished");
}

void GraphicsScene::shockwaveFinished(Node * pSender)
{
	//DBOUT("shockwave finished");
}

void GraphicsScene::paddleExplosionFinished(Node* pSender)
{
	paddle_sprite->setAnchorPoint(Vec2(0, 1));
	paddle_sprite->setPosition(WINDOW_WIDTH/2, paddle_level);
	paddle_sprite->setRotation(0);
	explosion_in_progress = false;
	CCLOG("Paddle animation finished");
}

// располагаем репеллеры по периметру
void GraphicsScene::loadrepellers()
{	
	auto lf = [this](int x, int y)
	{
		auto repeller_sprite = Sprite::create("repeller.png");
		repeller_sprite->setAnchorPoint(Vec2(0.5, 0.5));
		repeller_sprite->setOpacity(100);
		repeller_sprite->setPosition(x, y);
		//repeller_sprite->retain();  //todo: delete
		if (!REPELLERS_VISIBLE)
			repeller_sprite->setVisible(false);
		this->addChild(repeller_sprite, 0);
		vRepellers.pushBack(repeller_sprite);

		return x + y;
	};

	for(int x = 0; x <= WINDOW_WIDTH; x+=40)
	{
		lf(x, repellers_lower_level);
		lf(x, WINDOW_HEIGHT);
	}
	
	for (int y = repellers_lower_level + 40; y <= WINDOW_HEIGHT; y += 40)
	{
		lf(0, y);
		lf(WINDOW_WIDTH, y);
	}

}



void GraphicsScene::gameLogic(float dt)
{
	if (!attack_in_progress)
	{
		attack_in_progress = true;
		//select enemy to attack
		for (int j = 0; j < MAXY; j++)
			for (int i = 0; i < MAXX; i++)
			{

				if (pEnemies[i][j]) {
					//!!!! debug: атаковать только врагами одного типа type
					//!!!!
					//if (pEnemies[i][j]->type == enForce ) // todo: delete if!!!!
					{
						attackingEnemy = pEnemies[i][j];
						attackingEnemy->attack();
						j = MAXY;
						break;
					}

				}
			}
	}

	//атакующий маневр

	if (attackingEnemy) {
		if (attackingEnemy->type == enMultiplier) {
			auto callback = CallFuncN::create([=](Node* Sender) {
				auto action = MoveBy::create(1.0f, Vec2(0, rand() % 20 - 10));
				this->attackingEnemy->runAction(action);
			});
		}
		else if (attackingEnemy->type == enBasic || attackingEnemy->type == enDestoyer) {
			auto callback = CallFuncN::create([=](Node* Sender){
				auto action = MoveBy::create(1.0f, Vec2(0, rand() % 20 - 10));
				this->attackingEnemy->runAction(action);
				this->attackingEnemy->fire();
			});

			auto action = MoveTo::create(2.0f, Vec2(paddle_sprite->getPosition().x + paddle_width / 2, paddle_sprite->getPosition().y+100));
			auto action1 = ScaleTo::create(0.5f, 1.5f);
			auto spawn = Spawn::create(action, action1, NULL);
			auto seq = Sequence::create(action, callback, NULL);
			attackingEnemy->runAction(seq);

			/*Vec2 random_push = Vec2((rand() % 20 - 10), (rand() % 50 - 100));
			attackingEnemy->speed_vector += random_push;*/

		}
		else if  (attackingEnemy->type == enForce) { 
			Vec2 speed = Vec2(paddle_sprite->getPosition().x + paddle_width / 2.0f, paddle_sprite->getPosition().y + 100)
				- attackingEnemy->getPosition();
			Vec2 speedNorm = speed.getNormalized();
			attackingEnemy->speed_vector.x = 110 * speedNorm.x;
			attackingEnemy->speed_vector.y = 50 * speedNorm.y;

			if (getPosition().x > paddle_sprite->getPosition().x + 10 && getPosition().x < paddle_sprite->getPosition().x - 10) {
				if (attackingEnemy->type != enMultiplier)
					attackingEnemy->fire();
			}

			Vec2 random_push = Vec2((rand() % 20 - 10), (rand() % 50 - 100));
			//attackingEnemy->speed_vector += random_push;
		}

	}
	else {
		CCLOG("No attacking enemy");
	}
}

void GraphicsScene::shieldUp(float)
{
	if(shield < MAXSHIELD)
	  shield++;

}

void GraphicsScene::moveEnemies(float)
{
	int minx = 0;	
	int miny = 200;
	int maxx = WINDOW_WIDTH - MyEnemyNode -> width;
	int maxy = WINDOW_HEIGHT - MyEnemyNode -> height;
	int rangeX = maxx - minx;
	int rangeY = maxy - miny;

	int x = rand() % rangeX + minx;
	int y = rand() % rangeY + miny;

	auto action = MoveTo::create(4.0f, Vec2(x	, y));
	MyEnemyNode->runAction(action);

	//MyEnemyNode->runAction(Sequence::create(action1,
	//	CallFuncN::create( [&](Node* Sender) {GraphicsScene::movingEnemiesFinished(Sender); }), NULL));
}

void GraphicsScene::checkEnemyCollision(EnemySprite *enSprite, BallSprite* ball_sprite, Vec2 &ball_position)
{
	Vec2 &vBall_speed = ball_sprite->vBall_speed;

	float raidus = ball_sprite->raidus;

	//пересчитываемкоординаты спрайта врага в координаты сцены

	Size enemySize = enSprite->size;
	Vec2 enemypos;

	if (enSprite->attacking) {
		enemypos = enSprite->getPosition();
	}
	else
	{
		enemypos = MyEnemyNode->convertToWorldSpace(enSprite->getPosition());
	}
	
	Rect EnemyRect_scene = Rect(enemypos, enemySize);

	float diffx = ball_position.x - enemypos.x;
	float diffy = ball_position.y - enemypos.y;
	float distance = pow(diffx, 2) + pow(diffy, 2);
	
	mindist = pow(enSprite->radius + raidus, 2);

	if (distance <= mindist) {
		if (MUSIC)
			AudioEngine::play2d("hitenemy.mp3", false, 0.01f);

		Enemy2* p = dynamic_cast <Enemy2*> (enSprite);
		if (p)
		{
			//ball_sprite = BallSprite::create();
			//ball_sprite->setPosition(300, 400);
			//ball_sprite->vBall_speed = Vec2(-50, -100);
			//vBalls.pushBack(ball_sprite);
			//this->addChild(ball_sprite, 2);
		}

		//force calculation
		float mag_ball = pow(vBall_speed.x, 2) + pow(vBall_speed.y, 2);
		float mag_enemy = (pow(enSprite->speed_vector.x, 2) + pow(enSprite->speed_vector.y, 2));
		float force = sqrt(mag_ball + mag_enemy);
		//float force = sqrt(mag_ball);
		float angle = atan2(diffy, diffx);
		vBall_speed.x = force * cos(angle);
		vBall_speed.y = force * sin(angle);

		//ball_position.x = ball_position.x + (enSprite->radius + raidus + sgn(force)) * cos(angle);
		//ball_position.y = ball_position.y + (enSprite->radius + raidus + sgn(force)) * sin(angle);

		//проверка на повторное срабатывание столкновения
		// enemy добавляется в вектор vSpritesToDelete на короткое время if hp = 2 и надолго :-) если hp = 1 
		// (реализовано в EnemySprite::decreaseHealth)
		Vector<EnemySprite*>::iterator myIterator;
		
		//если столкновение с enSprite уже обрабатывается - выходим из функции
		for (myIterator = vSpritesToDelete.begin(); myIterator != vSpritesToDelete.end(); ++myIterator)
		{
			if (enSprite == *myIterator)
				return;
		}

		enSprite->decreaseHealth();

	}
	
}

void GraphicsScene::restart()
{

	//enemy удаляются в callback try-sprite animation из decreaseHealth()
	score = 0;

	Vector<EnemySprite*>::iterator myIterator;

	//удаляем лишние шарики
	Vector<BallSprite*>::iterator itball;
	for (itball = vBalls.begin(); itball != vBalls.end(); ++itball)
	{
		this->removeChild(*itball);
	}

	vBalls.clear();
	Vector<BulletSprite*>::iterator myi;

	for (myi = vBullets.begin(); myi != vBullets.end(); ++myi)
	{
		this -> removeChild(*myi);
	}

	BallSprite* ball_sprite = BallSprite::create();
	ball_sprite->setPosition(50, 50);
	vBalls.pushBack(ball_sprite);
	this->addChild(ball_sprite, 2);

	try_sprite->setScale(0.1f);
	try_sprite->setVisible(false);
	win_sprite->setScale(0.1f);
	win_sprite->setVisible(false);
	GenerateArmy();
	health = MAXHEALTH;
	shield = MAXSHIELD;
	attack_in_progress = false;
	paddle_speed = 100;

}
// создает построение заполняет 2d масив pEnemies и вектор vAllSprites
bool GraphicsScene::GenerateArmy()
{
	EnemySprite* enemy_sprite;

	for (int j = 0; j < MAXY; j++)
	{
		for (int i = 0; i < MAXX; i++)
		{

			if (i % 4 == 0)
				enemy_sprite = Enemy1::create();
			else if (i % 4 == 1)
				enemy_sprite = Enemy2::create();
			else if (i % 4 == 2)
				enemy_sprite = Enemy3::create();
			else if (i % 4 == 3)
				enemy_sprite = Enemy4::create();

			if (enemy_sprite) {
				enemy_sprite->setAnchorPoint(Vec2(0.5, 0.5));
				enemy_sprite->setPosition(15 + 50 * i, 15 + 50 * j);
				enemy_sprite->grid_x = i;
				enemy_sprite->grid_y = j;
				pEnemies[i][j] = enemy_sprite;
				vAllSprites.pushBack(enemy_sprite);
				//enemy_sprite->retain(); // todo delete release при удалении цели 
				MyEnemyNode->addChild(enemy_sprite);
			}
			else
			{
				DBOUT("Can't create enemy sprite");

				return false;
			}

		}

	}

	//MyEnemyNode->calcBoundaries();
	//float x = WINDOW_WIDTH - MyEnemyNode->width;
	//float  y = WINDOW_HEIGHT - MyEnemyNode->height;
	MyEnemyNode->setPosition(Vec2(100, 400));

	return true;

}

void GraphicsScene::explodePaddle()
{
	explosion_in_progress = true;
	if (MUSIC)
		AudioEngine::play2d("pexpl.mp3", false, 1.0f);

	paddle_sprite->setAnchorPoint(Vec2(0.5, 0.5));
	paddle_position = Vec2(paddle_sprite->getPosition().x + paddle_width / 2, paddle_sprite->getPosition().y /*paddle_level + 100*/);
	paddle_sprite->setPosition(paddle_position);
	auto action1 = MoveBy::create(1, Vec2(-0, 30));
	auto action2 = RotateBy::create(1, -100);
	auto spawn = Spawn::create(action1, action2, paddle_explosion, NULL);
	//paddle_sprite->runAction(spawn);

	auto seq = Sequence::create(spawn, CallFunc::create([=]() 
	{
			explosion_in_progress = false;
	}), NULL);

	paddle_sprite->runAction(seq);

}

void GraphicsScene::decreaseHealth()
{
	health--;
	if (health < 1) {
		Vector<EnemySprite*>::iterator myIterator;

		if (attackingEnemy) {
			this->removeChild(attackingEnemy);
			vAllSprites.eraseObject(attackingEnemy);
			attackingEnemy->release();
			attackingEnemy = NULL;
		}

		for (myIterator = vAllSprites.begin(); myIterator != vAllSprites.end(); ++myIterator)
		{
			EnemySprite* spr = *myIterator;
			pEnemies[spr->grid_x][spr->grid_y] = NULL;
			MyEnemyNode->removeChild(spr);
			spr->release();
		}

		vAllSprites.clear();

		auto delayAction = DelayTime::create(3.0f);
		auto myScale = ScaleTo::create(0.5f, 1.0f);
		auto myScale1 = ScaleTo::create(3.0f, 1.0f);
		//auto myScale2 = ScaleTo::create(3.0f, 0.1f);
		try_sprite->setVisible(true);
		//health = shield = 0;
		try_sprite->runAction(Sequence::create(myScale, myScale1, CallFunc::create([=]() { GraphicsScene::restart(); }), NULL));
	
	}

}

void GraphicsScene::shieldDown()
{
	if (!explosion_in_progress) {
		shield--;
		if (MUSIC)
			AudioEngine::play2d("bomb.mp3", false, 1.0f);
		if (shield < 0) {
			shield = MAXSHIELD;
			decreaseHealth();
			explodePaddle();
		}
	}

}
