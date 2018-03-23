#pragma once
#include "cocos2d.h"
//#include "SimpleAudioEngine.h"
#include "MySprites.h"
#include "EnemyNode.h"
#include <vector>
USING_NS_CC;

class GraphicsScene : public cocos2d::Layer
{
public:
	~GraphicsScene();
	
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GraphicsScene);

	float paddle_width;						// ширина платформы
	bool explosion_in_progress = false;		// флаг взрыв цели - в прощессе	

	Vector <BulletSprite*> vBullets;		//летящие снаряды
	Vector <EnemySprite*> vSpritesToDelete;	// цели которые будут удалены
	Vector <EnemySprite*>	vAllSprites;	// все цели 
	Vector <BallSprite*>	vBalls;			// шарики
	Vector <BallSprite*>	vBallsToDelete;	// шарики к удалению
	Vector <Sprite*>		vRepellers;		// репеллеры периметра

	EnemySprite *pEnemies[MAXX][MAXY];		// массив построения
	EnemyNode *MyEnemyNode;					// узел содержащий все спрайты в построении
	EnemySprite * attackingEnemy = NULL;	// атакующая цель

	Animation * animation_force_expand;		// анимация разворачивания силового поля
	Animation * animation_shockwave;
	Animation * animation_bexpl;			//взрыв шарика

	Sprite* paddle_sprite;					// бита
	Sprite* shockwave_sprite;
	Sprite* bullet_sprite;
	Sprite* win_sprite;
	Sprite* try_sprite;

	Sequence* createExplosionSeq();			//create sequence for enemy explosion
	Sequence* createBallExplosionSeq();		//create sequence for ball explosion

	//callbackfunctions
	void bulletExplosionFinished(Node * pSender);		// callback взрыв снаряда завершен
	void shockwaveFinished(Node * pSender);				// callback ударная волна завершена
	void paddleExplosionFinished(Node * pSender);		// вpрыв платформы завершен
	void tryagainFinished(Node* pSender);				// callback try gain завершен
	void fireMultiplier(float);
	void fireall(float);
	void loadrepellers();
	void update(float) override;


private:
	SpriteFrame * frame;

	bool attack_in_progress = false;			// флаг атаки (цель вышла из построения и атакует)
	bool bLeft;									// paddle motion triggers if bLeft==true - paddle moves to the left 
	bool bRight;								// paddle motion triggers if bLeft==true - paddle moves to the left 
	int   repellers_lower_level = 140;
	int   health = MAXHEALTH;					// максимальное кол-во жизней платформы
	int   shield = MAXSHIELD;					// макс значение защиты платформы
	int   score = 0;
	int   opacity = 255;						// прозрачность цели
	int   paddle_level = 30;					// верхний уровень платформы
	float Kenemy = 5000.0;						//коэффициент антигравитации цели
	float Krep = 100.0;							//коэффициент антигравитации репеллера
	float paddle_speed = 100;					// скорость платформы
	float mindist;								// минимальное расстояние цель-шарик

	Vec2 paddle_position;
	Vec2 oldpaddle_position;					// position of the paddle from previous update
					
	Label *labelHealth, *labelShield, *labelScore;

	void GraphicsScene::gameLogic(float);		// периодически вызываемая callback функция, запускающая события игры
	void GraphicsScene::shieldUp(float);		// периодически вызываемая callback функция, запускающая события игры       
	void GraphicsScene::moveEnemies(float);		// периодически вызываемая callback функция, запускающая события игры       
	void explodePaddle();						// взрывает платформу
	void decreaseHealth();						// шменьшает жизни, проверяет на <0
	void shieldDown();
	void restart();								// перезапуск уровня
	bool GenerateArmy();						//создает армию инвэйдеров

	//callback functions
	void movingEnemiesFinished(Node * pSender);	// callback сдвиг построения завершен
	void enemyExplosionFinished(Node* pSender);	//  callback взрыв цели завершен

	int		sgn(double x);							//signum function
	float	tohead(Vec2 vector);					//get heading angle of a vector - преобразование к углу от направления вверх
	void	checkEnemyCollision(EnemySprite*, BallSprite*, Vec2 &ball_positio); // проверка столкновений целей с шариком

	//Animations and sequences	
	Animation * animation_enemy_expl;
	Animation * animation_bullet;
	Animation * animation_paddle;
	Sequence* explosion;					//sequence взрыв цели 
	Sequence* paddle_explosion;				//sequence взрыв платформы 
	
};