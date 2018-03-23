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

	float paddle_width;						// ������ ���������
	bool explosion_in_progress = false;		// ���� ����� ���� - � ��������	

	Vector <BulletSprite*> vBullets;		//������� �������
	Vector <EnemySprite*> vSpritesToDelete;	// ���� ������� ����� �������
	Vector <EnemySprite*>	vAllSprites;	// ��� ���� 
	Vector <BallSprite*>	vBalls;			// ������
	Vector <BallSprite*>	vBallsToDelete;	// ������ � ��������
	Vector <Sprite*>		vRepellers;		// ��������� ���������

	EnemySprite *pEnemies[MAXX][MAXY];		// ������ ����������
	EnemyNode *MyEnemyNode;					// ���� ���������� ��� ������� � ����������
	EnemySprite * attackingEnemy = NULL;	// ��������� ����

	Animation * animation_force_expand;		// �������� �������������� �������� ����
	Animation * animation_shockwave;
	Animation * animation_bexpl;			//����� ������

	Sprite* paddle_sprite;					// ����
	Sprite* shockwave_sprite;
	Sprite* bullet_sprite;
	Sprite* win_sprite;
	Sprite* try_sprite;

	Sequence* createExplosionSeq();			//create sequence for enemy explosion
	Sequence* createBallExplosionSeq();		//create sequence for ball explosion

	//callbackfunctions
	void bulletExplosionFinished(Node * pSender);		// callback ����� ������� ��������
	void shockwaveFinished(Node * pSender);				// callback ������� ����� ���������
	void paddleExplosionFinished(Node * pSender);		// �p��� ��������� ��������
	void tryagainFinished(Node* pSender);				// callback try gain ��������
	void fireMultiplier(float);
	void fireall(float);
	void loadrepellers();
	void update(float) override;


private:
	SpriteFrame * frame;

	bool attack_in_progress = false;			// ���� ����� (���� ����� �� ���������� � �������)
	bool bLeft;									// paddle motion triggers if bLeft==true - paddle moves to the left 
	bool bRight;								// paddle motion triggers if bLeft==true - paddle moves to the left 
	int   repellers_lower_level = 140;
	int   health = MAXHEALTH;					// ������������ ���-�� ������ ���������
	int   shield = MAXSHIELD;					// ���� �������� ������ ���������
	int   score = 0;
	int   opacity = 255;						// ������������ ����
	int   paddle_level = 30;					// ������� ������� ���������
	float Kenemy = 5000.0;						//����������� �������������� ����
	float Krep = 100.0;							//����������� �������������� ���������
	float paddle_speed = 100;					// �������� ���������
	float mindist;								// ����������� ���������� ����-�����

	Vec2 paddle_position;
	Vec2 oldpaddle_position;					// position of the paddle from previous update
					
	Label *labelHealth, *labelShield, *labelScore;

	void GraphicsScene::gameLogic(float);		// ������������ ���������� callback �������, ����������� ������� ����
	void GraphicsScene::shieldUp(float);		// ������������ ���������� callback �������, ����������� ������� ����       
	void GraphicsScene::moveEnemies(float);		// ������������ ���������� callback �������, ����������� ������� ����       
	void explodePaddle();						// �������� ���������
	void decreaseHealth();						// ��������� �����, ��������� �� <0
	void shieldDown();
	void restart();								// ���������� ������
	bool GenerateArmy();						//������� ����� ����������

	//callback functions
	void movingEnemiesFinished(Node * pSender);	// callback ����� ���������� ��������
	void enemyExplosionFinished(Node* pSender);	//  callback ����� ���� ��������

	int		sgn(double x);							//signum function
	float	tohead(Vec2 vector);					//get heading angle of a vector - �������������� � ���� �� ����������� �����
	void	checkEnemyCollision(EnemySprite*, BallSprite*, Vec2 &ball_positio); // �������� ������������ ����� � �������

	//Animations and sequences	
	Animation * animation_enemy_expl;
	Animation * animation_bullet;
	Animation * animation_paddle;
	Sequence* explosion;					//sequence ����� ���� 
	Sequence* paddle_explosion;				//sequence ����� ��������� 
	
};