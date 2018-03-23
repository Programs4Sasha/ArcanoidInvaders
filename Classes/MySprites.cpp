#include "MySprites.h"
#include "GraphicsScene.h"
#include "AudioEngine.h"
using namespace cocos2d::experimental;

//*************************************************************************
//Class EnemySprite
// ласс врагов базовый дл€ классов Enemy1, Enemy2....
//*************************************************************************

EnemySprite::EnemySprite()
{
}

EnemySprite::~EnemySprite()
{
}

EnemySprite* EnemySprite::create(const std::string& filename)
{
	EnemySprite *sprite = new (std::nothrow) EnemySprite();
	if (sprite && sprite->initWithFile(filename))
	{
		sprite->autorelease();
		sprite->size = sprite->getContentSize();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void EnemySprite::setPosition(const Vec2 & pos)
{
	Sprite::setPosition(pos);
	if (!oldPosition.equals(pos)) {
		oldPosition = pos;
	}
}

void EnemySprite::setPosition(float x, float y)
{
	Vec2 pos = Vec2(x, y);
	Sprite::setPosition(x, y);
	if (!oldPosition.equals(pos)) {
		oldPosition = pos;
	}
}

void EnemySprite::fire()
{
	float bullet_speed = 100.0f;
	int bullposy = 10;
	int bullposx = 10;
	BulletSprite *bullet_sprite; // 

	Node* MyEnemyNode = (Node*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene")->getChildByName("MyEnemyNode");
	
	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
							->getChildByName("GraphicsScene");

	if (Scene->explosion_in_progress) { 
		return; 
	}
	
	if (attacking)
	{
		bullposy = this->getPosition().y - ENEMY_RADIUS - 2;
		bullposx = this->getPosition().x;
	}
	else
	{
		bullposy = MyEnemyNode->convertToWorldSpace(this->getPosition()).y - ENEMY_RADIUS - 2;
		bullposx = MyEnemyNode->convertToWorldSpace(this->getPosition()).x;
	}

	bullet_sprite = BulletSprite::create();
	bullet_sprite->setAnchorPoint(Vec2(0.5, 0.5));
	bullet_sprite->setPosition(bullposx, bullposy);
	//bullet_sprite->retain(); 
	//todo: delete release

	Scene->addChild(bullet_sprite);
	Scene->vBullets.pushBack(bullet_sprite);
	bullet_sprite->runAction(createBulletSequence());
}

void EnemySprite::attack()
{
	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene");

	EnemySprite*& attackingEnemy = Scene->attackingEnemy;

	Scene->pEnemies[attackingEnemy->grid_x][attackingEnemy->grid_y] = NULL; //убиираем атакующего из массива построени€

	attackingEnemy->retain(); //!!! удал€етс€ в enemyExplosionFinished при убийстве атакующего 
	Vec2 pos = Scene->MyEnemyNode->convertToWorldSpace(attackingEnemy->getPosition());

	Scene->MyEnemyNode->removeChild(attackingEnemy);
	attackingEnemy->setPosition(pos);
	attackingEnemy->setAnchorPoint(Vec2(0.5, 0.5));
	attackingEnemy->speed_vector = Vec2(-10, 10);

	attackingEnemy->runAction(ScaleTo::create(0.5f, 1.5f));
	attackingEnemy->radius *= 1.5f;
	Scene->addChild(attackingEnemy, 3);
}

void EnemySprite::decreaseHealth()
{

	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())->getChildByName("GraphicsScene");


	if (hp == 2)
	{
		setOpacity(150);
		hp--;
		Scene->vSpritesToDelete.pushBack(this);

		// удалениеиз из вектора vSpritesToDelete с задержкой 0.2
		// 
		auto delayAction = DelayTime::create(0.2f);
		auto funcCallback = CallFunc::create([=]() {Scene->vSpritesToDelete.eraseObject(this); });
		runAction(Sequence::create(delayAction, funcCallback, NULL));

	}
	else if (hp == 1)
	{
		setOpacity(255);
		Scene->vSpritesToDelete.pushBack(this);

		if (MUSIC)
			AudioEngine::play2d("Explosion.mp3", false, 1.0f);

		Sequence* seq = Scene->createExplosionSeq();
		if (seq)
			this->runAction(Scene->createExplosionSeq());

	}
}

Sequence* EnemySprite::createBulletSequence()
{
	float bullet_speed = 100.0f;


	float bullposy;
	float bullposx;
	

	if (attacking)
	{
		bullposy = this->getPosition().y - ENEMY_RADIUS - 2;
		bullposx = this->getPosition().x;

	}
	else
	{
		Node* MyEnemyNode = (Node*)(Director::getInstance()->getRunningScene())->getChildByName("GraphicsScene")
			->getChildByName("MyEnemyNode");
		bullposy = MyEnemyNode->convertToWorldSpace(this->getPosition()).y - ENEMY_RADIUS - 2;
		bullposx = MyEnemyNode->convertToWorldSpace(this->getPosition()).x;
	}


	//float bullposy = this->getPosition().y - ENEMY_RADIUS - 2;
	//float bullposx = this->getPosition().x;

	CallFuncN * callback;
	auto action = MoveTo::create(bullposy / bullet_speed, Vec2(bullposx, 0));

	callback = CallFuncN::create([=](Node* Sender) {
		GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
			->getChildByName("GraphicsScene");

		Sprite *sprite = dynamic_cast<Sprite*>(Sender);
		Scene->removeChild(Sender);
		Scene->vBullets.eraseObject((BulletSprite*)Sender);
		CCLOG("Bullet flight finished");
	});

	return Sequence::create(action, callback, NULL);
}



//*************************************************************************
//Class Enemy1
//
//ѕри атаке производит одиночные выстрелы
//type = enBasic
//*************************************************************************
Enemy1::Enemy1()
{
	type = enBasic;
}

Enemy1* Enemy1::create()
{
	Enemy1 *sprite = new (std::nothrow) Enemy1();
	if (sprite && sprite->initWithFile("enemy0.png"))
	{
		sprite->autorelease();
		sprite->size = sprite->getContentSize();
		sprite->type = enBasic;
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Enemy1::attack()
{

	EnemySprite::attack(); // там перенос Enemy# из MyEnemyNode в GraphicsScene

	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
							->getChildByName("GraphicsScene");
	attacking = true;

	auto action1 = MoveBy::create(2, Vec2(0, -20));

	this->runAction(Sequence::create(action1,
		CallFuncN::create([=](Node* Sender) 
		{
			auto action = MoveBy::create(0.5f, Vec2(0, rand() % 20 - 10));
			Scene->attackingEnemy->runAction(action);
			//Scene->attackingEnemy->fire();
			CCLOG("Attack maneuver finished");
		}), NULL));
}

//*************************************************************************
//Class Enemy2
//
//ѕри атаке разворачивает силовое поле
//type = enForce
//*************************************************************************
Enemy2::Enemy2()
{
	capturedBall = NULL;
	type = enForce;
	radius = 16.5f;
	force = Sprite::create("force\\01.png");
	force->setAnchorPoint(Vec2(0.5, 0.5));
	//force->setOpacity(200);
	force->setScale(2 / 3.0f);
	force->setVisible(true);
	//shockwave->setPosition(radius, radius);
	force->setPosition(17, 12);
	this->addChild(force, 0);
}

Enemy2* Enemy2::create()
{
	Enemy2 *sprite = new (std::nothrow) Enemy2();
	if (sprite && sprite->initWithFile("enemy1.png"))
	{
		sprite->autorelease();
		sprite->size = sprite->getContentSize();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Enemy2::attack()
{
	EnemySprite::attack(); // там перенос Enemy# из MyEnemyNode в GraphicsScene

	auto action1 = MoveTo::create(1, Vec2(getPosition().x, 250));
	//auto delayAction = DelayTime::create(0.5f);
	attacking = true;
	auto funcCallback = CallFunc::create([=]() {
		//after delay
		GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
			->getChildByName("GraphicsScene");

		Vec2 paddle_pos = Scene->paddle_sprite->getPosition();
		Vec2 speed = Vec2(paddle_pos.x + Scene->paddle_width / 2, paddle_pos.y + 200)
			- Scene->attackingEnemy->getPosition();
		Scene->attackingEnemy->speed_vector = 100 * speed.getNormalized();
		deploy_force();

	});

	this->runAction(Sequence::create(action1, /*delayAction,*/ funcCallback, NULL));
}

void Enemy2::deploy_force()
{
	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene");

	force->runAction(Sequence::create(CCAnimate::create(Scene->animation_force_expand),
		CallFunc::create([=]()
		{
			CCLOG("Force field deploy finished");
		}
		), NULL));

}

//*************************************************************************
//Class Enemy3
//
//ѕри атаке производит от 3 до 5 выстрелов
//type = enDestroyer
//*************************************************************************

Enemy3::Enemy3()
{
	type = enDestoyer;

}

Enemy3* Enemy3::create()
{
	
	Enemy3 *sprite = new (std::nothrow) Enemy3();
	if (sprite && sprite->initWithFile("enemy2.png"))
	{
		sprite->autorelease();
		sprite->size = sprite->getContentSize();
		sprite->type = enDestoyer;
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Enemy3::attack()
{
	EnemySprite::attack(); // там перенос Enemy# из MyEnemyNode в GraphicsScene

	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene");
	attacking = true;

	auto action1 = MoveBy::create(2.0f, Vec2(0, -20));

	this->runAction(Sequence::create(action1,
		CallFuncN::create([=](Node* Sender)
	{
		auto action = MoveBy::create(2.0f, Vec2(0, rand() % 20 - 10));
		Scene->attackingEnemy->runAction(action);
		Scene->attackingEnemy->fire();
		CCLOG("Attack maneuver finished");
	}), NULL));
}

void Enemy3::fire()
{
	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene");

	DelayTime* delay[5];
	for (int i = 0; i < 5; i++)
	{
		delay[i] = DelayTime::create(float(0.4 + i * 0.3));
	}

	srand(time(NULL));
	int numbullets = rand() % 3 + 3;

	for (int c = 0; c < numbullets; c++)
	{
		this->runAction(Sequence::create(delay[c], CallFuncN::create([=](Node* Sender)
		{
			float bullposy = this->getPosition().y - ENEMY_RADIUS - 2;
			float bullposx = this->getPosition().x;
			auto bullet_sprite = BulletSprite::create();
			bullet_sprite->setPosition(bullposx, bullposy);
			Scene->addChild(bullet_sprite);
			Scene->vBullets.pushBack(bullet_sprite);
			Sequence* seq = createBulletSequence();
			bullet_sprite->runAction(seq);
		}), NULL));
	}
}

void Enemy3::decreaseHealth()
{
	Vec2 destroyerPos;
	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene");

	if (this->attacking) {
		if (hp == 1) {
			destroyerPos = this->getPosition();
			Scene->shockwave_sprite->setVisible(true);
			Scene->shockwave_sprite->setPosition(this->getPosition());
			auto anim = CCAnimate::create(Scene->animation_shockwave);
			auto scale = ScaleTo::create(2, 4);
			auto spawn = Spawn::create(anim, scale, NULL);

			Scene->shockwave_sprite->runAction(Sequence::create(spawn,
				CallFunc::create([=]()
			{
				Scene->shockwave_sprite->setVisible(false);
				Scene->shockwave_sprite->setScale(1.0);
				Vector<EnemySprite*>::iterator en;
				std::sort(Scene->vAllSprites.begin(), Scene->vAllSprites.end(), [=](EnemySprite* a, EnemySprite* b) {
					float dist1 = (Scene->MyEnemyNode->convertToWorldSpace(a->getPosition()) - destroyerPos).getLength();
					float dist2 = (Scene->MyEnemyNode->convertToWorldSpace(b->getPosition()) - destroyerPos).getLength();
					return dist1 < dist2; //от меньшего к большему
				});

				int count = 0;
				for (en = (Scene->vAllSprites).begin(); en != (Scene->vAllSprites).end(); ++en)
				{

					EnemySprite* curEnemy = *en;
					if (curEnemy == this)
						continue;

					if (count < 10)
					{
						curEnemy->decreaseHealth();
						count++;
					}
				}

				CCLOG("shockwave animation finished");
			}
			), NULL));

		}
	}

	EnemySprite::decreaseHealth();
}

//*************************************************************************
//Class Enemy4
//ѕри атаке не стрел€ет, а создает объекты типов Enemy1(type = enBasic) и 
//Enemy3(type= enDestoyer) с интервалом MULTIPLIER_INTERVAL (#define в Mysprites.h)
//
//type = enMultipoier
//*************************************************************************

Enemy4::Enemy4()
{
	type = enMultiplier;
}

Enemy4* Enemy4::create()
{
	Enemy4 *sprite = new (std::nothrow) Enemy4();
	if (sprite && sprite->initWithFile("enemy3.png"))
	{
		sprite->autorelease();
		sprite->size = sprite->getContentSize();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void Enemy4::attack()
{
	EnemySprite::attack(); // там перенос Enemy# из MyEnemyNode в GraphicsScene

	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene");

	attacking = true;

	auto action1 = MoveBy::create(2, Vec2(0, -20));

	this->runAction(Sequence::create(action1,
		CallFuncN::create([=](Node* Sender)
	{
		auto action = MoveBy::create(0.5f, Vec2(0, rand() % 20 - 10));
		Scene->attackingEnemy->runAction(action);
		//Scene->attackingEnemy->fire();
		CCLOG("Attack maneuver finished");
	}), NULL));
}

void Enemy4::fire()
{
	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene");

	EnemySprite* enemy_sprite;
	int sel =0;
	srand(time(NULL));
	if (createdEnemies < 4)
	{
		for (int j = 0; j < MAXY; j++)
		{
			for (int i = 0; i < MAXX; i++)
			{
				if (Scene->pEnemies[i][j] == NULL)
				{

					sel = rand() % 2;
					if (sel == 0)
						enemy_sprite = Enemy1::create();
					else
						enemy_sprite = Enemy3::create();

					if (enemy_sprite) {
						enemy_sprite->setAnchorPoint(Vec2(0.5, 0.5));
						enemy_sprite->setPosition(15 + 50 * i, 15 + 50 * j);
						enemy_sprite->grid_x = i;
						enemy_sprite->grid_y = j;
						Scene->pEnemies[i][j] = enemy_sprite;
						Scene->vAllSprites.pushBack(enemy_sprite);
						//enemy_sprite->retain(); 
						//todo: release
						Scene->MyEnemyNode->addChild(enemy_sprite);

						auto myScale = ScaleTo::create(0.5f, 0.3f);
						auto myScale1 = ScaleTo::create(0.5f, 1.5f);
						auto myScale2 = ScaleTo::create(0.5f, 1.0f);
						enemy_sprite->runAction(Sequence::create(myScale, myScale1, myScale2, NULL));
						createdEnemies++;
					}
					else
					{
						CCLOG("Can't create enemy sprite");
						return;
					}
					
					j = MAXY; break;
				}
			}
		}
	}
}

//*********************************************************************
// Class BallSprite 
//
// Ўарики
//*********************************************************************

BallSprite::BallSprite()
{
	vBall_speed = Vec2(50, 50);
}

BallSprite::~BallSprite()
{
}

BallSprite* BallSprite::create()
{
	BallSprite *sprite = new (std::nothrow) BallSprite();
	if (sprite && sprite->initWithFile("ball.png"))
	{
		sprite->setAnchorPoint(Vec2(0.5, 0.5));
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

void BallSprite::explode()
{

	GraphicsScene* Scene = (GraphicsScene*)(Director::getInstance()->getRunningScene())
		->getChildByName("GraphicsScene");

	BulletSprite* bspr = BulletSprite::create();
	bspr->setPosition(getPosition());
	Scene->addChild(bspr);
	bspr->runAction(Scene->createBallExplosionSeq());
}

//*************************************************************************
//Class BulletSprite
//
// —нар€ды
//*************************************************************************
BulletSprite::BulletSprite()
{
	vBullet_speed = Vec2(0, -50);
}

BulletSprite::~BulletSprite()
{
}

BulletSprite* BulletSprite::create()
{
	BulletSprite *sprite = new (std::nothrow) BulletSprite();
	if (sprite && sprite->initWithFile("bullet.png"))
	{
		sprite->setAnchorPoint(Vec2(0.5, 0.5));
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return nullptr;
}

