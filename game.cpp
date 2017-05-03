#include "stdafx.h"
#include <math.h>
#include "Antares\\antares.h"
#include "Antares\\timer.h"
#include "Antares\\controls.h"
#include "Antares\\crossdetect.h"
#include "Antares\\font.h"
#include "FileCrc.h"
#include "game.h"
#include "gameobjects.h"

const unsigned int SCREEN_WIDTH         = 800;
const unsigned int SCREEN_HEIGHT        = 600;
const unsigned short MAINT_TITLE_CRC16  = 0xE51C; // CRC16 ����� ����� (DAT) �������� 

//#define OBJECT_DEBUG    // �������� ���������������� ��������� �������� ���������� ���������� �� ������������ ��������
#define LIFE_BAR_X      5
#define LIFE_BAR_Y      SCREEN_HEIGHT - 36
#define FPS_X           560
#define FPS_Y           SCREEN_HEIGHT - 31
#define SCORE_X         705
#define SCORE_Y         SCREEN_HEIGHT - 31


const unsigned int INITIAL_PLAYER_LIVES = 3;    // ��������� ���-�� ������ ������
const unsigned int MAX_FRAMED_OBJECTS	= 50;   // ���-�� �������� ��������
const unsigned int MAX_ENEMY_TYPES		= 30;
const unsigned int MAX_ENEMY_OBJECTS	= 100;
const unsigned int MAX_MISSLE_TYPES		= 5;
const unsigned int MAX_MISSLE_OBJECTS	= 100;
const unsigned int MAX_EFFECT_TYPES		= 10;
const unsigned int MAX_EFFECT_OBJECTS	= 200;
const unsigned int MAX_STAR_OBJECTS		= 40;
const unsigned int MAX_STAR_SPEED		= 100;
const float ROOT2                       = 0.704f; 
const float TIME_BEFORE_SHOW_GAME_OVER  = 3.0f; // ����� ����� ������� ������� ������ � ������� ��������� � ������ ����
const float ASK_CONTINUE_DELAY          = 1.0f;
const float TIME_FOR_SHOW_LEVEL_NUMBER  = 2.0f; // �����, � ������� �������� ������������ ����� � ������� ������ 
const float PAUSE_BEFORE_SHOW_LEVEL     = 0.5f; // ��������� ����� ����� ������� ������ � ������� ������
const float TIME_SHOW_THE_END           = 9.0f; // �����, � ������� �������� ������������ ������� ��������� ���� 

// ������������ ��������
enum{
    EXPLOSION = 0,
    SMOKE,
};

CTimer  timer;
CMouse	mouse;
CKeys	keys;
CFont font;
CSprite playerSprite;
CSprite missleSprite;
CSprite effectSpr;
CSprite enemySprite;
CSprite mainTitleImage;
CFramedObject fobject[MAX_FRAMED_OBJECTS];
GameObjectList<CEnemy, MAX_ENEMY_OBJECTS> enemyList;
GameObjectList<CMissle, MAX_MISSLE_OBJECTS> missleList;
GameObjectList<CEffect, MAX_EFFECT_OBJECTS> effectList;
GameObjectList<CStar, MAX_STAR_OBJECTS> starList;
CPlayer player;
CEnemy  enemy[MAX_ENEMY_TYPES];
CMissle missle[MAX_MISSLE_TYPES];
CEffect	effect[MAX_EFFECT_TYPES];
CCrossMask missleMask;
CCrossMask spriteMask;
CCrossMask playerMask;
CCrossMask enemyMask;
CNumbers number;
CLifeIndicator lifeIndicator;
CLevel level;
CLevelList levelList;
CGameMenu menu[MENU_AMOUNT];
CColorFlash colorFlash1; // ������ ��� ������� "��� ������ ����..."
CColorFlash colorFlash2; // ������ ��� ������� "�����" � ����
CColorFlash colorFlash3; // ������ ��� ������ � ��������� ���� 


// ��� ������� � ����� ������� �� ����� �� ���� 
extern CExitControl exitControl;

//#define FILE_DEBUG
#ifdef FILE_DEBUG
FILE *fptr;
#endif


// ������ ������ CGameView (��� ����� � ������������� ��������� ������ � game.h)
//// 
CGameView::CGameView(CTimer *timer)
{
	pauseDuration = 0;
	pauseStartTime = 0;
    gameStartError = false; // ����� ����� ������ ������� ����

    // ��������� ��������� �� ������� ������ ������� 
    this->timer = timer;
}

////
bool CGameView::getGameStartError()
{return gameStartError;}

////
void CGameView::setGameStartError(bool isError)
{gameStartError = isError;}

///
int CGameView::getState()
{return state;}

////
void CGameView::setState(int state)
{

    // ���� ������������� � ��������� �����, ���� �������� ������ �� ����� ���� (� ������������ ����� � ������� ����), 
    // �� ����� ��������� ������� ������� ���������� �� ����� 
    if (this->state == PLAY && (state == PAUSE || state == ASK_TO_ABORT))
        pauseStartTime = timer->getTime();

    // ���� ������� ��������� - ����� � [���������� ������ �-��] ���� ������� ����� � ����� (������� � 
    // ����� ���� - PLAY), �� ������� ��������� ������������ �������, � ������� �������� ���������� � 
    // ��������� ����� 
    if ((this->state == PAUSE || this->state == ASK_TO_ABORT) && state == PLAY)
        pauseDuration = timer->getTime() - pauseStartTime;


    // ���������� ������ ��������� ���� 
    this->state = state;
}

/////
float CGameView::getPauseDuration()
{return pauseDuration;}

/////
float CGameView::getTimeNewPhase()
{return timeNewPhase;}

////
void CGameView::setTimeNewPhase(float timeNewPhase)
{this->timeNewPhase = timeNewPhase;}

//// ��������� �������� �������� ����� ������� ������
void CGameView::prepareForPlayLevel(float time)            
{
    // set up player position & speed
    player.init(390.0f, 30.0f, 150.0f, 150.0f);

    // ������� kill() ��������� ������ ��������� ������� � ������ ���������� 
    // � ���������� ��������
    CEnemy tempEnemy;
    for (unsigned int i = 0; i < enemyList.getElementsNumber(); i++)
    {
        tempEnemy = enemyList.getObject(i);
        tempEnemy.kill();
        enemyList.reloadObject(i, tempEnemy);
    }

    // ������� kill() ��������� ������ ������ � ������ ���������� 
    // � ���������� ��������
    CMissle tempMissle;
    for (unsigned int i = 0; i < missleList.getElementsNumber(); i++)
    {
        tempMissle = missleList.getObject(i);
        tempMissle.kill();
        missleList.reloadObject(i, tempMissle);
    }

    // ������� kill() ��������� ������ ������ � ������ ���������� 
    // � ���������� ��������
    CEffect tempEffect;
    for (unsigned int i = 0; i < effectList.getElementsNumber(); i++)
    {
        tempEffect = effectList.getObject(i);
        tempEffect.kill();
        effectList.reloadObject(i, tempEffect);
    }

    // �������� ������ �������� ������
    level.load(levelList.fileName[level.currentLevel]);

    // ������������ ������ � ��������� ������ 
    sprintf_s(level.stringName, "������� %u\n", level.currentLevel);

    // ����� ������ ����� ���� �������� ������� ������� ������ ������
    level.currentEntry = 0;
    
    // �������� ������� ������� ��� ������� ���������, ����� �������� ����� ������������ ������ ������ ������, 
    // ����� ����������� � ����� ����������� ������ ������ SHOW_LEVEL_NUMBER ��� ��������� ��������� PLAY  
    

    // ����� ������� ������ (��������� ����� ��������� ��������, ����� � ����� ������, ��������, ������ ������������ 
    // �������, � �� ����� ����� �� ����������� ������ ���������� ������ ���� ��������. ����� ��� ������ ������ ������ 
    // [���� ���� ������] ��������, ����������� �� ������ ������ ����� ����������� �� ������� �� ���) 
    keys.setPressed(UP_ARROW_BUTTON, false);
    keys.setPressed(DOWN_ARROW_BUTTON, false);
    keys.setPressed(LEFT_ARROW_BUTTON, false);
    keys.setPressed(RIGHT_ARROW_BUTTON, false);
    keys.setPressed(Z_BUTTON, false);
}

CGameView gameView(&timer);

///////////////////////////////////
////       Main classes        ////
///////////////////////////////////
void CGame::init(void)
{
    #ifdef FILE_DEBUG
    fptr = fopen("debug.txt", "wt");
    #endif

	CStar	tempStar;
   
	playerSprite.loadDAT("Media\\player.dat");
	playerSprite.createSprite();

	enemySprite.loadDAT("Media\\enemyShip.dat");
	enemySprite.createSprite();

	missleSprite.loadDAT("Media\\missle.dat");
	missleSprite.createSprite();

	effectSpr.loadDAT("Media\\effects.dat");
	effectSpr.createSprite();

    font.loadMap("Media\\font.rfm");
    font.loadImageDat("Media\\font.dat");

    // �������� ����������� ����� ���� � ������������ �������� 
    if (fileCrc16("Media\\mainTitle.dat", 0xFFFF) != MAINT_TITLE_CRC16)
        gameView.setGameStartError(true);

    mainTitleImage.loadDAT("Media\\mainTitle.dat");
    mainTitleImage.createSprite();

	missleMask.load("Media\\missleNew.msk");
	spriteMask.load("Media\\sprite.msk");
	playerMask.load("Media\\player.msk"); 
	enemyMask.load("Media\\enemyShip.msk");

    // ������������� ������ ���� � ������������ �������
    number.init(effectSpr, 0, 50, 14, 21);
    
    // ������������� ���� ����� ������ ���������� ����� (alive / dead)
    lifeIndicator.init(90, 123, 51, 81, ALIVE_TILE, effectSpr);
    lifeIndicator.init(124, 157, 51, 81, DEAD_TILE, effectSpr);

	///////////////////////////////
	// framed objects init

	//// players object
	fobject[0].initFrames(3);
	fobject[0].setFrame(0,  0, 47,  0, 47);
	fobject[0].setFrame(1, 48, 95,  0, 47);
	fobject[0].setFrame(2,  0, 47, 48, 95);

	//// enemy 1 (hunter) - ������� "�������"
	fobject[1].initFrames(3);
	fobject[1].setFrame(0,  0,  47, 0, 55);
	fobject[1].setFrame(1, 48,  95, 0, 55);
	fobject[1].setFrame(2, 96, 143, 0, 55);

	//// enemy 2 (asteroid)
	fobject[2].initFrames(12);
	fobject[2].setFrame( 0,   0,  41, 104, 145);
    fobject[2].setFrame( 1,  42,  83, 104, 145);
    fobject[2].setFrame( 2,  84, 125, 104, 145);
    fobject[2].setFrame( 3, 126, 167, 104, 145);
	fobject[2].setFrame( 4, 168, 209, 104, 145);
    fobject[2].setFrame( 5, 210, 251, 104, 145);
    fobject[2].setFrame( 6,   0,  41, 146, 187);
    fobject[2].setFrame( 7,  42,  83, 146, 187);
    fobject[2].setFrame( 8,  84, 125, 146, 187);
    fobject[2].setFrame( 9, 126, 167, 146, 187);
    fobject[2].setFrame(10, 168, 209, 146, 187);
    fobject[2].setFrame(11, 210, 251, 146, 187);

	//// enemy 3 (supervisor) - ���������� "�������"
	fobject[3].initFrames(6);
	fobject[3].setFrame(0,   0,  39, 188, 255);
    fobject[3].setFrame(1,  40,  79, 188, 255);
    fobject[3].setFrame(2,  80, 119, 188, 255);
    fobject[3].setFrame(3, 120, 159, 188, 255);
    fobject[3].setFrame(4, 160, 199, 188, 255);
    fobject[3].setFrame(5, 200, 239, 188, 255);

	//// enemy 4 (turret)
	fobject[4].initFrames(8);
	fobject[4].setFrame(0,   0,  46, 303, 349); // up
	fobject[4].setFrame(1,  47,  93, 303, 349); // up-right
	fobject[4].setFrame(2,  94, 140, 303, 349); // right
	fobject[4].setFrame(3, 141, 187, 303, 349); // down-right
	fobject[4].setFrame(4,   0,  46, 256, 302); // down
	fobject[4].setFrame(5,  47,  93, 256, 302); // down-left
	fobject[4].setFrame(6,  94, 140, 256, 302); // left
	fobject[4].setFrame(7, 141, 187, 256, 302); // up-left

	//// enemy 5 (scout) - �����
	fobject[5].initFrames(3);
	fobject[5].setFrame(0,  0,  47, 56, 103);
	fobject[5].setFrame(1, 48,  95, 56, 103);
	fobject[5].setFrame(2, 96, 143, 56, 103);

    //// enemy 6 (probe) - ���� "�����"
    fobject[6].initFrames(4);
    fobject[6].setFrame(0, 144, 183,  0,  51);
    fobject[6].setFrame(1, 184, 223,  0,  51);
    fobject[6].setFrame(2, 144, 183, 52, 103);
    fobject[6].setFrame(3, 184, 223, 52, 103);

    //// enemy 7 (cruser) - ������� 
    fobject[7].initFrames(2);
    fobject[7].setFrame(0, 256, 392,   0, 147);
    fobject[7].setFrame(1, 256, 392, 148, 295);


	//// missle 1 (player missle)
	fobject[10].initFrames(3);
	fobject[10].setFrame(0,  0, 31, 0, 47);
    fobject[10].setFrame(1, 32, 63, 0, 47);
    fobject[10].setFrame(2, 64, 95, 0, 47);

	//// missle 2 (enemy missle)
	fobject[11].initFrames(3);
    fobject[11].setFrame(0,  0, 31, 48, 95);
    fobject[11].setFrame(1, 32, 63, 48, 95);
    fobject[11].setFrame(2, 64, 95, 48, 95);

	//// missle 3 (enemy missle)
	fobject[12].initFrames(4);
    fobject[12].setFrame(0,  0,  31, 96, 127);
    fobject[12].setFrame(1, 32,  63, 96, 127);
    fobject[12].setFrame(2, 64,  95, 96, 127);
    fobject[12].setFrame(3, 96, 127, 96, 127);

    //// missle 4 (enemy missle) violet
    fobject[13].initFrames(12);
    fobject[13].setFrame( 0, 160, 207,   0, 47);
    fobject[13].setFrame( 1, 208, 255,   0, 47);
    fobject[13].setFrame( 2, 160, 207,  48, 95);
    fobject[13].setFrame( 3, 208, 255,  48, 95);
    fobject[13].setFrame( 4,   0,  47, 128, 175);
    fobject[13].setFrame( 5,  48,  95, 128, 175);
    fobject[13].setFrame( 6,  96, 143, 128, 175);
    fobject[13].setFrame( 7, 144, 191, 128, 175);
    fobject[13].setFrame( 8,   0,  47, 176, 223);
    fobject[13].setFrame( 9,  48,  95, 176, 223);
    fobject[13].setFrame(10,  96, 143, 176, 223);
    fobject[13].setFrame(11, 144, 191, 176, 223);

    //// missle 5 (enemy missle) turrer missle
    fobject[14].initFrames(2);
    fobject[14].setFrame(0,  96, 111, 0, 15);
    fobject[14].setFrame(1, 112, 127, 0, 15);


	//// effect 0 (explosion 0)
	fobject[20].initFrames(10);
	fobject[20].setFrame(0,   0,  47, 144, 191);
	fobject[20].setFrame(1,  48,  95, 144, 191);
	fobject[20].setFrame(2,  96, 143, 144, 191);
	fobject[20].setFrame(3, 144, 191, 144, 191);
	fobject[20].setFrame(4, 192, 239, 144, 191);
    fobject[20].setFrame(5,   0, 47,  192, 239);
    fobject[20].setFrame(6,  48, 95,  192, 239);
    fobject[20].setFrame(7,  96, 143, 192, 239);
    fobject[20].setFrame(8, 144, 191, 192, 239);
    fobject[20].setFrame(9, 192, 239, 192, 239);

    
    //// ������ 1 (��� 1) 
    fobject[21].initFrames(5);
    fobject[21].setFrame(0,   0,  47, 96, 143);
    fobject[21].setFrame(1,  48,  95, 96, 143);
    fobject[21].setFrame(2,  96, 143, 96, 143);
    fobject[21].setFrame(3, 144, 191, 96, 143);
    fobject[21].setFrame(4, 192, 239, 96, 143);

	//// star frames
	fobject[23].initFrames(3);
	fobject[23].setFrame(0,  90,  97, 88, 95);
	fobject[23].setFrame(1,  98, 105, 88, 95);
	fobject[23].setFrame(2, 106, 113, 88, 95);

	//// platforms (�������� ��������)
    fobject[30].initFrames(1);
    fobject[30].setFrame(0,   0,  39, 350, 389); // ���� �����-���

    fobject[31].initFrames(1);
    fobject[31].setFrame(0,  40,  79, 350, 389); // ���� ����-��� 

    fobject[32].initFrames(1);
    fobject[32].setFrame(0,  80, 119, 350, 389); // ���� ����-����

    fobject[33].initFrames(1);
    fobject[33].setFrame(0, 120, 159, 350, 389); // ���� ����-�����
////
    fobject[34].initFrames(1);
    fobject[34].setFrame(0,   0,  39, 390, 429); // ���� ���-�����

    fobject[35].initFrames(1);
    fobject[35].setFrame(0,  40,  79, 390, 429); // ���� ���-����

    fobject[36].initFrames(1);
    fobject[36].setFrame(0,  80, 119, 390, 429); // ���� ����-����

    fobject[37].initFrames(1);
    fobject[37].setFrame(0, 120, 159, 390, 429); // ���� ����-�����
////
    fobject[38].initFrames(1);
    fobject[38].setFrame(0,   0,  39, 430, 469); // ���

    fobject[39].initFrames(1);
    fobject[39].setFrame(0,  40,  79, 430, 469); // ����

    fobject[40].initFrames(1);
    fobject[40].setFrame(0,  80, 119, 430, 469); // ����

    fobject[41].initFrames(1);
    fobject[41].setFrame(0, 120, 159, 430, 469); // �����
////
    fobject[42].initFrames(1);
    fobject[42].setFrame(0, 160, 199, 350, 389); // �����

    // endof framed objects init
    ///////////////////////////////


	//// player init
	player.setImage(playerSprite);
	player.setMask(playerMask);
	player.setFrames(&fobject[0]);
	player.setMissleInterval(0.2f);
	player.setDeathTime(0.4f);
	player.setHitPoints(10); 
	player.setCrossingDamage(50);
	// player init routine will be called later

	// init enemy type 0 (hunter) - "�������"
	enemy[0].setImage(enemySprite);
	enemy[0].setMask(enemyMask);
	enemy[0].setFrames(&fobject[1]);
	enemy[0].setHitPoints(10);
    enemy[0].setScore(20);
	enemy[0].setCrossingDamage(20); // damage for player if there is crossing 
	enemy[0].setDeathTime(0.3f);
	enemy[0].setAnimateType(NORMAL_ANIMATED);
	enemy[0].setFireType(FIRE_DOWN_DIRECTION);
	enemy[0].setFireInterval(0.5f);
	enemy[0].setMissleNumber(1); 
    enemy[0].setFireProbability(8); // ����������� �������� ���� 80% 
    enemy[0].setSmokeControl(4, 0.4f, 0, 20); // ��������� ���������� ��������� ���� 
    
	// init enemy type 1 (asteroid)
    enemy[1].setImage(enemySprite);
	enemy[1].setMask(enemyMask);
	enemy[1].setFrames(&fobject[2]);
	enemy[1].setHitPoints(5);
    enemy[1].setScore(5);
	enemy[1].setCrossingDamage(20); // damage for player if there is crossing 
	enemy[1].setDeathTime(0.3f);
	enemy[1].setAnimateType(CICLED_ANIMATED);
	enemy[1].setAnimateInterval(0.1f);
	enemy[1].setFireType(NOT_FIRE);

	// init enemy type 2 (supervisor) - "�������"
    enemy[2].setImage(enemySprite);
	enemy[2].setMask(enemyMask);
	enemy[2].setFrames(&fobject[3]);
	enemy[2].setHitPoints(10);
    enemy[2].setScore(30);
	enemy[2].setCrossingDamage(20); // damage for player if there is crossing 
	enemy[2].setDeathTime(0.3f);
    enemy[2].setAnimateType(CICLED_ANIMATED);
    enemy[2].setAnimateInterval(0.05f);
	enemy[2].setFireType(FIRE_ANY_DIRECTION);
	enemy[2].setFireInterval(1.5f);
	enemy[2].setMissleNumber(2);
    enemy[2].setFireProbability(8); // ����������� �������� ���� 80% 

	// init enemy type 3 (turret)
	enemy[3].setImage(enemySprite);
	enemy[3].setMask(enemyMask);
	enemy[3].setFrames(&fobject[4]);
	enemy[3].setHitPoints(6);
    enemy[3].setScore(10);
	enemy[3].setCrossingDamage(20); // damage for player if there is crossing 
	enemy[3].setDeathTime(0.01f);
	enemy[3].setAnimateType(NOT_ANIMATED);
	enemy[3].setFireType(FIRE_AS_TURRET);
	enemy[3].setFireInterval(0.8f);
	enemy[3].setMissleNumber(4);
	enemy[3].setCurrentFrame(TURRET_FRAME_DOWN);

	// init enemy type 4 (scout) - "�����"
	enemy[4].setImage(enemySprite);
	enemy[4].setMask(enemyMask);
	enemy[4].setFrames(&fobject[5]);
	enemy[4].setHitPoints(3);
    enemy[4].setScore(15);
	enemy[4].setCrossingDamage(20); // damage for player if there is crossing 
	enemy[4].setDeathTime(0.3f);
	enemy[4].setAnimateType(NORMAL_ANIMATED);
	enemy[4].setFireType(FIRE_DOWN_DIRECTION);
	enemy[4].setFireInterval(1.5f);
	enemy[4].setMissleNumber(1);
    enemy[4].setFireProbability(7); // ����������� �������� ���� 70% 

    // init enemy type 5 (probe) - "����"
    enemy[5].setImage(enemySprite);
    enemy[5].setMask(enemyMask);
    enemy[5].setFrames(&fobject[6]);
    enemy[5].setHitPoints(3);
    enemy[5].setScore(10);
    enemy[5].setCrossingDamage(20); // damage for player if there is crossing 
    enemy[5].setDeathTime(0.3f);
    enemy[5].setAnimateType(PINPONG_ANIMATED);
    enemy[5].setAnimateInterval(0.2f);
    enemy[5].setFireType(NOT_FIRE);

    // init enemy type 6 (cruser)
    enemy[6].setImage(enemySprite);
    enemy[6].setMask(enemyMask);
    enemy[6].setFrames(&fobject[7]);
    enemy[6].setHitPoints(40);
    enemy[6].setScore(60);
    enemy[6].setCrossingDamage(20); // damage for player if there is crossing 
    enemy[6].setDeathTime(0.3f);
    enemy[6].setAnimateType(CICLED_ANIMATED);
    enemy[6].setAnimateInterval(0.2f);
    enemy[6].setFireType(NOT_FIRE);
    enemy[6].setSmokeControl(20, 0.4f, 0, 30); // ��������� ���������� ��������� ���� 

    // init enemy type 10-23 (platform)
    for (int i = 0; i < 13; i++)
    {
        enemy[i + 10].setImage(enemySprite);
        enemy[i + 10].setMask(enemyMask);
        enemy[i + 10].setFrames(&fobject[i + 30]);
        enemy[i + 10].setHitPoints(100);
        enemy[i + 10].setCrossingDamage(20); // damage for player if there is crossing 
        enemy[i + 10].setDeathTime(0.3f);
        enemy[i + 10].setAnimateType(NOT_ANIMATED);
        enemy[i + 10].setFireType(NOT_FIRE);
    }
    

	//// init missle types

    // ������ (�����)
	missle[0].setImage(missleSprite);
	missle[0].setMask(missleMask);
	missle[0].setFrames(&fobject[10]);
	missle[0].setDamage(3);
	missle[0].setSpeedValue(300.0f);
	missle[0].setTargetType(AGAINST_ENEMY);

    // �������
	missle[1].setImage(missleSprite);
	missle[1].setMask(missleMask);
	missle[1].setFrames(&fobject[11]);
	missle[1].setDamage(20);
	missle[1].setSpeedValue(300.0f);
	missle[1].setTargetType(AGAINST_PLAYER);
	missle[1].setAnimateInterval(0.2f);

    // ������� ������
	missle[2].setImage(missleSprite);
	missle[2].setMask(missleMask);
	missle[2].setFrames(&fobject[12]);
	missle[2].setDamage(20);
	missle[2].setSpeedValue(300.0f);
	missle[2].setTargetType(AGAINST_PLAYER);
	missle[2].setAnimateInterval(0.3f);

    // ���������� �������
    missle[3].setImage(missleSprite);
    missle[3].setMask(missleMask);
    missle[3].setFrames(&fobject[13]);
    missle[3].setDamage(20);
    missle[3].setSpeedValue(200.0f);
    missle[3].setTargetType(AGAINST_PLAYER);
    missle[3].setAnimateInterval(0.1f);

    // ������ ������� 
    missle[4].setImage(missleSprite);
    missle[4].setMask(missleMask);
    missle[4].setFrames(&fobject[14]);
    missle[4].setDamage(20);
    missle[4].setSpeedValue(300.0f);
    missle[4].setTargetType(AGAINST_PLAYER);
    missle[4].setAnimateInterval(0.2f);


	//// effect 0 init (�����)
	effect[0].setImage(effectSpr);
	effect[0].setFrames(&fobject[20]);
	effect[0].setFramePeriod(0.1f);

    //// effect 1 init (���)
    effect[1].setImage(effectSpr);
    effect[1].setFrames(&fobject[21]);
    effect[1].setFramePeriod(0.1f);

	/////////////

	//// init stars
    tempStar.setImage(effectSpr);
	tempStar.setFrames(&fobject[23]);

	int j;
	for (int i = 0; i < MAX_STAR_OBJECTS; i++)
	{
		j = rand() % 3;
		tempStar.init(j, (float)(rand() % SCREEN_WIDTH), (float)(rand() % SCREEN_HEIGHT), -(float)(MAX_STAR_SPEED >> j));
		starList.addObject(tempStar);
	}

   
    // ������������ ������� ������� ����
    levelList.add("level1.txt");
    levelList.add("level2.txt");
    levelList.add("level3.txt");

    // �������� ������ ������ ������������ � �-�� CGameView::prepareForPlayLevel()

	//// ������������ � ����������� �������� ���� 
    gameView.setState(MAIN_TITLE);


    //// ������������� ���������� ���� 
    ////
    CGameMenu::changeItemDelay = 0.2f; // ��������� �������� ��� �������� ����� �������� ���� 
    CGameMenu::img = effectSpr;        // ����������� ������ ��� ����������� ��������� ���� 
    
    // ���� � �������� �� ������� � �������� ������
    menu[MENU_EXIT_TO_MAIN].addItem(320, 320, 160);
    menu[MENU_EXIT_TO_MAIN].addItem(320, 280, 160);
    menu[MENU_EXIT_TO_MAIN].setSelectorRect(175, 206, 51, 82); // ���������� ����� ��������� � ������ 

    // ���� � �������� �� ����� � ������� 
    menu[MENU_EXIT_TO_SYSTEM].addItem(303, 230, 192);
    menu[MENU_EXIT_TO_SYSTEM].addItem(303, 190, 192);
    menu[MENU_EXIT_TO_SYSTEM].setSelectorRect(175, 206, 51, 82); // ���������� ����� ��������� � ������ 

    
    //// ���������������� �������� ��� ���������� �� ������� ������ �������� �����
    colorFlash1.init(0x000000FF, 0x0000FFFF, 1.0f, timer.getTime()); // ������ ��� ������� "��� ������ ����..."
    colorFlash2.init(0x00FF0000, 0x000000FF, 1.0f, timer.getTime()); // ������ ��� ������� "�����" � ����
    colorFlash3.init(0x00FFFFFF, 0x0000FF00, 0.5f, timer.getTime()); // ������ ��� ������ � ��������� ����

}

////
void CGame::uninit(void)
{
    // game uninitialization

    #ifdef FILE_DEBUG
    fclose(fptr);
    #endif

	missleSprite.deleteSprite();
	enemySprite.deleteSprite();
	effectSpr.deleteSprite();
    mainTitleImage.deleteSprite();

	missleMask.deleteMask();
	spriteMask.deleteMask();
	playerMask.deleteMask();
	enemyMask.deleteMask();
	
	fobject[0].uninitFrames();	// posible move to destructor???
	fobject[1].uninitFrames();	// posible move to destructor???
	fobject[2].uninitFrames();	// posible move to destructor???
	fobject[3].uninitFrames();	// posible move to destructor???
	fobject[4].uninitFrames();	// posible move to destructor???
	fobject[5].uninitFrames();	// posible move to destructor???
    fobject[6].uninitFrames();	// posible move to destructor???
    fobject[7].uninitFrames();	// posible move to destructor???
	fobject[10].uninitFrames(); // posible move to destructor???
	fobject[11].uninitFrames(); // posible move to destructor???
	fobject[12].uninitFrames(); // posible move to destructor???
    fobject[13].uninitFrames(); // posible move to destructor???
    fobject[14].uninitFrames(); // posible move to destructor???
	fobject[20].uninitFrames(); // posible move to destructor???
	fobject[23].uninitFrames(); // posible move to destructor???

	
	font.uninit();
}

////////////////////////
//	    Rendering     //
////////////////////////
void CGame::render(void)
{
	CEnemy temp;
	CMissle tempMissle;
	CEffect tempEffect;
	CStar tempStar;
	CCrossMask tempEnemyMask;
	CCrossMask tempPlayerMask;
	CCrossMask tempMissleMask;
	RECT r1, r2;
	unsigned int i, j, k;
	float xp, yp, xe, ye, hyp, spd; // temp
	int	eol = 0; // compare with {EOF = (-1)}
	
	struct{
		float x;
		float y;
	} a, b;	// for turret aiming

	// get time delta from last frame
	float dt = timer.getDeltaTime();

    static unsigned int fps; // ������� ������

    // ���� ��������� � ��������� ���� � ����� ������ exitControl ��������� ��������� 
    // � ���, ��� ����� ��������� �� �����, ��...
    if (gameView.getState() == PLAY && exitControl.isPause() == true) 
        gameView.setState(PAUSE);

    // ������ ������� ������, ���������� ��� �������� ������ �� ����� 
    if (((timer.getTime() - level.lastEntryTime) > level.entry[level.currentEntry].timeWait)
        && (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER) 
        && level.currentEntry < level.readedEntries)
    {
        // ���� ��� �������� ������� ���������� �������� ������ (���� ������ ���-�� �������� � ��������� ������) 
        if (level.entry[level.currentEntry].amount == 0)
        {
            // ���� ����������� ������� �� �������� ��������� ������� ����
            if (level.currentLevel < levelList.addedLevels)
            {
                // ��������� ��������� ������� � �������� ���������� ������ 
                gameView.prepareForPlayLevel(timer.getTime());

                gameView.setState(SHOW_LEVEL_NUMBER);
                gameView.setTimeNewPhase(timer.getTime());

                // �������� ������ ���� �����, ���� � ���� �� �� ������������ ����� � ������� ��������� ������
                player.addLife();


            }else // ��������� ������� ����������� ���������� ������ ���� 
            {
                //
                //  THE END
                //
                gameView.setState(THE_END);
                gameView.setTimeNewPhase(timer.getTime());
            }
        }

        int type, tType;
        unsigned char tDir, lStat;
        float x, y, vxMax, vxMin, xAmp, vy;

        // ���� �������� �������� CEnemy ������� ������ ������ 
        for (i = 0; i < level.entry[level.currentEntry].amount; i++)
        {
            // ��������� ���������� �������� �������
            type    = level.entry[level.currentEntry].levelObject[i].type;
            x       = level.entry[level.currentEntry].levelObject[i].x;
            y       = level.entry[level.currentEntry].levelObject[i].y;
            tType   = level.entry[level.currentEntry].levelObject[i].tType;
            vxMax   = level.entry[level.currentEntry].levelObject[i].vxMax;
            vxMin   = level.entry[level.currentEntry].levelObject[i].vxMin;
            xAmp    = level.entry[level.currentEntry].levelObject[i].xAmp;
            vy      = level.entry[level.currentEntry].levelObject[i].vy;
            tDir    = level.entry[level.currentEntry].levelObject[i].tDir;
            lStat   = level.entry[level.currentEntry].levelObject[i].lStat;

            // ������������� ���������� ������� CEnemy
            enemy[type].init(x, y, tType, vxMax, vxMin, xAmp, vy);

            // �� ������, ���� ��������� �������� �������� - ��������� ����������� ����������� �������� 
            enemy[type].setTurretDirections(tDir);

            // ��������� ����� �������: ��� ����� / �������� ������ / ����������� ������ 
            enemy[type].setLinkType(lStat);

            enemyList.addObject(enemy[type]);
        }

        
        // ��������� ������� ������� 
        level.lastEntryTime = timer.getTime();

        // ��������� �������� �������������� ������� �������� CEnemy 
        level.currentEntry++;
    }


	////////////////////
	// process star list
	for (i = 0; i < starList.getElementsNumber(); i++)
	{
		tempStar = starList.getObject(i);

        if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER || 
            gameView.getState() == THE_END)
            tempStar.processMovement(dt);

        if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER
            || gameView.getState() == PAUSE || gameView.getState() == ASK_TO_ABORT || gameView.getState() == THE_END)
        {
            tempStar.draw();
        }

		starList.reloadObject(i, tempStar);

		// delete effect if it is not active
		if (tempStar.isAlive() == false)
		{
			j = rand() % 3;
			tempStar.init(j, (float)(rand() % SCREEN_WIDTH), 610.0f, -(float)(MAX_STAR_SPEED >> j));
			starList.reloadObject(i, tempStar);
		}
	}


	///////////////////////////////////////////////
	// process enemy list [for movement processing]
    k = 0; // ������ ������� � ��������� enemyList 
	for (i = 0; i < enemyList.getElementsNumber(); i++)
	{
		temp = enemyList.getObject(k);

        if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER || 
            gameView.getState() == THE_END)
            temp.processMovement(dt);

        if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER
            || gameView.getState() == PAUSE || gameView.getState() == ASK_TO_ABORT || gameView.getState() == THE_END)
        {
            temp.draw();
        }

        // ��������: ����� �� ��������� �� ����� ��������� ���� 
        if (temp.processSmoke(&timer) == true && (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || 
            gameView.getState() == SHOW_GAME_OVER || gameView.getState() == THE_END))
        {
            effect[SMOKE].init((float)temp.getSmokeX(), (float)temp.getSmokeY(), 0, 0, 0, 1); // ������������ ������� ����
            effectList.addObject(effect[SMOKE]);	// ���������� ������� � ������� �������� �������� 
        }

		// ENEMY UNIT FIRE 
        if (((timer.getTime() - temp.getLastFireTime()) > temp.getFireInterval())
            && gameView.getState() == PLAY && temp.isEnemyAlive() == true)
		{
			// don't need to reset pause diration - it was reseted in enemy processing block
//            gameView.resetPauseDuration();

            // �������� �������� �������� (������), ������� ����� ������ �������� ��������������� ����������
			spd = missle[temp.getMissleNumber()].getSpeedValue();

			switch (temp.getFireType())
			{
				case FIRE_DOWN_DIRECTION:

					// is player on fire [vertical] line
					if ((unsigned int)(temp.getX() - player.getX0()) < player.getWidth() &&
						(temp.getY0() > player.getY()))
					{
						// ready to fire and aim
						temp.setLastFireTime(); // reload fire timer
                        
                        // ��������� ����������� �������� ����
                        if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                        {
                            missle[temp.getMissleNumber()].setSpeed(0, -spd);
                            missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                            missleList.addObject(missle[temp.getMissleNumber()]);
                        }
					}

					break;

				case FIRE_ANY_DIRECTION:

					// ready to fire and aim
					temp.setLastFireTime(); // reload fire timer

					xe = (float)temp.getX();
					ye = (float)temp.getY();
					xp = (float)player.getX();
					yp = (float)player.getY();

					hyp = sqrt((xe - xp)*(xe - xp) + (ye - yp)*(ye - yp));

					if (hyp != 0)
					{
                        // ��������� ����������� �������� ����
                        if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                        {
                            missle[temp.getMissleNumber()].setSpeed(-spd * (xe - xp) / hyp, -spd * (ye - yp) / hyp);
                            missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                            missleList.addObject(missle[temp.getMissleNumber()]);
                        }
					}

					break;

				case FIRE_AS_TURRET:
					
					if ((temp.getX() > (player.getX() - player.getWidth() / 2)) &&
						(temp.getX() < (player.getX() + player.getWidth() / 2)))
					{
						// if player locates above or under the enemy

						if (player.getY() > temp.getY())
						{
							if (temp.getTurretDirections() & TURRET_MASK_UP)
							{
								// ...above the enemy
								temp.setLastFireTime(); // reload fire timer
								temp.setCurrentFrame(TURRET_FRAME_UP);

                                // ��������� ����������� �������� ����
                                if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                                {
                                    missle[temp.getMissleNumber()].setSpeed(0, spd);
                                    missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                                    missleList.addObject(missle[temp.getMissleNumber()]);
                                }
							}
						} else
						{
							if (temp.getTurretDirections() & TURRET_MASK_DOWN)
							{
								// ...under the enemy
								temp.setLastFireTime(); // reload fire timer
								temp.setCurrentFrame(TURRET_FRAME_DOWN);

                                // ��������� ����������� �������� ����
                                if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                                {
                                    missle[temp.getMissleNumber()].setSpeed(0, -spd);
                                    missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                                    missleList.addObject(missle[temp.getMissleNumber()]);
                                }
							}
						}


					} else 
					if ((temp.getY() > (player.getY() - player.getHeight() / 2)) &&
						(temp.getY() < (player.getY() + player.getHeight() / 2)))
					{
						// if player locates right or left to the enemy

						if (player.getX() > temp.getX())
						{
							if (temp.getTurretDirections() & TURRET_MASK_RIGHT)
							{
								// ...right to the enemy
								temp.setLastFireTime(); // reload fire timer
								temp.setCurrentFrame(TURRET_FRAME_RIGHT);

                                // ��������� ����������� �������� ����
                                if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                                {
                                    missle[temp.getMissleNumber()].setSpeed(spd, 0);
                                    missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                                    missleList.addObject(missle[temp.getMissleNumber()]);
                                }
							}
						}
						else
						{
							if (temp.getTurretDirections() & TURRET_MASK_LEFT)
							{
								// ...left to the enemy
								temp.setLastFireTime(); // reload fire timer
								temp.setCurrentFrame(TURRET_FRAME_LEFT);

                                // ��������� ����������� �������� ����
                                if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                                {
                                    missle[temp.getMissleNumber()].setSpeed(-spd, 0);
                                    missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                                    missleList.addObject(missle[temp.getMissleNumber()]);
                                }
							}
						}
					}
					else 
					{
						// diagonals aiming
						// test if player locates up-left or down-right to the enemy
						// or player locates up-right or down-left to the enemy

						a.x = player.getX() - (float)(player.getWidth() / 2);
						a.y = player.getY() - (float)(player.getHeight() / 2);

						b.x = player.getX() + (float)(player.getWidth() / 2);
						b.y = player.getY() + (float)(player.getHeight() / 2);

						if ((b.y - temp.getY() > temp.getX() - b.x) && (a.y - temp.getY() < temp.getX() - a.x))
						{
							if (temp.getX() - player.getX() > player.getWidth() / 2)
							{
								if (temp.getTurretDirections() & TURRET_MASK_UP_LEFT)
								{
									// up-left
									temp.setLastFireTime(); // reload fire timer
									temp.setCurrentFrame(TURRET_FRAME_UP_LEFT);

                                    // ��������� ����������� �������� ����
                                    if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                                    {
                                        missle[temp.getMissleNumber()].setSpeed(-spd * ROOT2, spd * ROOT2);
                                        missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                                        missleList.addObject(missle[temp.getMissleNumber()]);
                                    }
								}
							}
							else
							if (player.getX() - temp.getX() > player.getWidth() / 2)
							{
								if (temp.getTurretDirections() & TURRET_MASK_DOWN_RIGHT)
								{
									// down-right
									temp.setLastFireTime(); // reload fire timer
									temp.setCurrentFrame(TURRET_FRAME_DOWN_RIGHT);

                                    // ��������� ����������� �������� ����
                                    if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                                    {
                                        missle[temp.getMissleNumber()].setSpeed(spd * ROOT2, -spd * ROOT2);
                                        missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                                        missleList.addObject(missle[temp.getMissleNumber()]);
                                    }
								}
							}
						}
						else
						{
							a.x = player.getX() - (float)(player.getWidth() / 2);
							a.y = player.getY() + (float)(player.getHeight() / 2);

							b.x = player.getX() + (float)(player.getWidth() / 2);
							b.y = player.getY() - (float)(player.getHeight() / 2);

							if ((a.y - temp.getY() > a.x - temp.getX()) && (b.x - temp.getX() > b.y - temp.getY()))
							{
								if (player.getX() - temp.getX() > player.getWidth() / 2)
								{
									if (temp.getTurretDirections() & TURRET_MASK_UP_RIGHT)
									{
										// up-right
										temp.setLastFireTime(); // reload fire timer
										temp.setCurrentFrame(TURRET_FRAME_UP_RIGHT);

                                        // ��������� ����������� �������� ����
                                        if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                                        {
                                            missle[temp.getMissleNumber()].setSpeed(spd * ROOT2, spd * ROOT2);
                                            missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                                            missleList.addObject(missle[temp.getMissleNumber()]);
                                        }
									}
								}
								else
								if (temp.getX() - player.getX() > player.getWidth() / 2)
								{
									if (temp.getTurretDirections() & TURRET_MASK_DOWN_LEFT)
									{
										// down-left
										temp.setLastFireTime(); // reload fire timer
										temp.setCurrentFrame(TURRET_FRAME_DOWN_LEFT);

                                        // ��������� ����������� �������� ����
                                        if (rand() % MAX_FIRE_PROBABILITY < (int)temp.getFireProbability())
                                        {
                                            missle[temp.getMissleNumber()].setSpeed(-spd * ROOT2, -spd * ROOT2);
                                            missle[temp.getMissleNumber()].init((float)temp.getX(), (float)temp.getY());
                                            missleList.addObject(missle[temp.getMissleNumber()]);
                                        }
									}
								}
							}
						}


					}

					break;
			}
		}

		r1 = player.getCurrentFrame();
		r2 = temp.getCurrentFrame();

		tempPlayerMask = player.getMask();
		tempEnemyMask = temp.getMask();

		tempPlayerMask.setRect(r1);
		tempEnemyMask.setRect(r2);


		// ENEMY UNIT CROSS PLAYER UNIT
		//
		if (crossDetect(player.getX0(), player.getY0(), tempPlayerMask, temp.getX0(), temp.getY0(), tempEnemyMask) &&
			temp.isPlayDeath() == false &&		// enemy is not playing death
			player.isBlinking() == false &&		// player is not in initial 'blinking' period
			player.isPlayDeath() == false &&	// player is not playing death
			player.isPlayerActive() == true)	// player is active
		{
			// set enemy hit from crossing with player
			temp.setHit(player.getCrossingDamage());
			// set player hit from crossing with enemy
			player.setHit(temp.getCrossingDamage());

			// if enemy got 'fatal' hit
			if (temp.isPlayDeath() == true)	// check if enemy 'killed' (HP decreased by players missle)  
			{
				effect[EXPLOSION].init((float)temp.getX(), (float)temp.getY(), 0, 0, 0, 1); // set explosion
				effectList.addObject(effect[EXPLOSION]);	// add the explosion to list of effects
			}

			// if player got 'fatal' hit
			if (player.isPlayDeath() == true)	// check if player 'killed' (HP decreased by players missle)  
			{
				effect[EXPLOSION].init((float)player.getX(), (float)player.getY(), 0, 0, 0.1f, 1); // set explosion
				effectList.addObject(effect[EXPLOSION]);	// add the explosion to list of effects
			}

		}

		enemyList.reloadObject(k, temp);	// save this enemy data in the enemy list

		// delete enemy if it is not alive
        if (temp.isEnemyAlive() == false)
            enemyList.removeObject(k);
        else
            k++; // �����, ���� �������� �������� enemyList �� ��������� - ��������� ������� ������� 
	}


	///////////////////////
	// PROCESS MISSLES LIST
    k = 0; // ������ ������� � ��������� missleList
	for (i = 0; i < missleList.getElementsNumber(); i++)
	{
		tempMissle = missleList.getObject(k);

        if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER || 
            gameView.getState() == THE_END)
            tempMissle.processMovement(dt);

        if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER
            || gameView.getState() == PAUSE || gameView.getState() == ASK_TO_ABORT || gameView.getState() == THE_END)
        {
            tempMissle.draw();
        }

		missleList.reloadObject(k, tempMissle);

		// delete missle if it is not alive
		if (tempMissle.isMissleAlive() == false)
			missleList.removeObject(k); // ������ ������ �� ������� missleList - ������ ������� �� �������� 
		else // else process crossing
		{

			// crossing detection block 

			// get current frame rectangle of current missle type
			r1 = tempMissle.getCurrentFrame();
			
			tempMissleMask = tempMissle.getMask();
			tempMissleMask.setRect(r1);

			if (tempMissle.getTargetType() == AGAINST_ENEMY)
			{
				// for each enemies: check for crossing with this missle
				for (j = 0; j < enemyList.getElementsNumber(); j++)
				{
					// get enemy from enemy list
					temp = enemyList.getObject(j);

					// for each enemy: get current frame rectangle
					r2 = temp.getCurrentFrame();

					tempEnemyMask = temp.getMask();
					tempEnemyMask.setRect(r2);

					if (crossDetect(temp.getX0(), temp.getY0(), tempEnemyMask, tempMissle.getX0(), tempMissle.getY0(), tempMissleMask) &&
						temp.isPlayDeath() == false) // checking 'play death state' for exeption hits after 
					{
						// Enemy has hitted by players missle!

						temp.setHit(tempMissle.getDamage());	// decrease enemy hitpoints & start flashing,  
						// also start 'play death' if hitpoints < 0

						if (temp.isPlayDeath() == true)	// check if enemy 'killed' (HP decreased by players missle)  
						{
							effect[0].init((float)temp.getX(), (float)temp.getY(), 0, 0, 0, 1); // set explosion
							effectList.addObject(effect[0]);	// add the explosion to list of effects

                            player.incrementScore(temp.getScore());
						}

						enemyList.reloadObject(j, temp);		// save state of the enemy in the list 

						tempMissle.setAliveStatus(false);		// status of this missle is 'killed'
						missleList.reloadObject(k, tempMissle); // update missle list

					}
				} // endfor processing list of enemy for crossing with missle
			} // endif target type of missle is AGAINST ENEMY
			else
			{ // if missle is AGAINST PLAYER

				r2 = player.getCurrentFrame();

				tempPlayerMask = player.getMask();
				tempPlayerMask.setRect(r2);

				// testing for touching any enemy with player
				if (crossDetect(player.getX0(), player.getY0(), tempPlayerMask, tempMissle.getX0(), tempMissle.getY0(), tempMissleMask) &&
					player.isBlinking() == false &&		// player is unhittable during blinking
					player.isPlayerActive() == true &&	// don't check hit when player not active
					player.isPlayDeath() == false)		// don't check hit when player play death
				{

					// reduce player HP after missle has hitted player
					player.setHit(tempMissle.getDamage()); 

					// if player got 'fatal' hit
					if (player.isPlayDeath() == true)	// check if enemy 'killed' (HP decreased by players missle)  
					{
						effect[EXPLOSION].init((float)player.getX(), (float)player.getY(), 0, 0, 0.1f, 1); // set explosion
						effectList.addObject(effect[EXPLOSION]);	// add the explosion to list of effects
					}

					tempMissle.setAliveStatus(false);		// status of this missle is 'killed'
					missleList.reloadObject(k, tempMissle); // update missle list
				}
			}
            
            k++; // ��������� ������� ��� ������� � ���������� �������� missleList
		}

	}


	////////////////////////
	// process player object
	
    if (gameView.getState() == PLAY || gameView.getState() == THE_END)
    {
        // ���� � ������ �������� �����
        if (player.getLivesNumber() > 0)
            player.processMovement(dt);
        else
        {   // ���� � ������ ����� �����������
            gameView.setState(LIFE_OVER);
            // ������ �������� ����� ��� ��������� ������� � ����� ����
            gameView.setTimeNewPhase(timer.getTime());
        }
    }
		
    // ������� ����������� ������� ������
    if (gameView.getState() == PLAY || gameView.getState() == PAUSE || gameView.getState() == ASK_TO_ABORT || 
        gameView.getState() == THE_END)
	    player.draw();
    ////////////////////////


    // ���� ��������� GAME OVER (����� ��������) � ������ ��������� �����, ����� ������� � ������ ������� � ����� ����
    if ((gameView.getState() == LIFE_OVER) && ((timer.getTime() - gameView.getTimeNewPhase()) > TIME_BEFORE_SHOW_GAME_OVER))
    {
        // ������������� � ����� ��������� ������� � ����� ����
        gameView.setState(SHOW_GAME_OVER);
        // ������������� ������� ������� ��� ������� ������� ����� ������� ������ "��� �����������..."
        gameView.setTimeNewPhase(timer.getTime());
    }       

    // ��������� ������� "����� ����"
    if (gameView.getState() == SHOW_GAME_OVER)
    {
        font.setColor(128, 255, 128);
        font.drawString(331, 400, (unsigned char*)"����� ����\n");

        if ((timer.getTime() - gameView.getTimeNewPhase()) > ASK_CONTINUE_DELAY)
        {
            colorFlash1.process(timer.getTime());
            font.setColor(colorFlash1.getRed(), colorFlash1.getGreen(), colorFlash1.getBlue());
             
            font.drawString(238, 300, (unsigned char*)"��� ����������� ����� <Enter>\n");
        }
    }

    // ��������� ����� ����
    if (gameView.getState() == MAIN_TITLE || gameView.getState() == READY_TO_START || gameView.getState() == ASK_TO_EXIT) 
    {
        // ��������� READY_TO_START ����� ������������� � ����� ��������� ������� �� ������ 
        // ����������� (������), ����� ��� ������� ������� � ��������� SHOW_LEVEL_NUMBER 

        // ���������� ����������� �������� ����
        RECT r; r.top = 0; r.bottom = 599; r.left = 0; r.right = 799;
        mainTitleImage.drawSprite(0, 0, r, NO_SCALE, NOT_REFLECT);

        if (gameView.getState() == ASK_TO_EXIT)
        {
            menu[MENU_EXIT_TO_SYSTEM].drawSelector(); // ���������� ��������

            colorFlash3.process(timer.getTime()); // ��������� ��������� �������
            
            if (menu[MENU_EXIT_TO_SYSTEM].getSelected() == 0)
                // ���� ������ 0-� ����� ����, �� �������� ������ ������� ������ � ���� �������
                font.setColor(colorFlash3.getRed(), colorFlash3.getGreen(), colorFlash3.getRed());
            else 
                // ���� ������ 0-� ����� ����, �� �������� ������ ������� ������ � ����������� ����
                font.setColor(128, 255, 128);

            font.drawString(317, 240, (unsigned char*)"����� � �������\n");

            if (menu[MENU_EXIT_TO_SYSTEM].getSelected() == 1)
                // ���� ������ 1-� ����� ����, �� �������� ������ ������� ������ � ���� �������
                font.setColor(colorFlash3.getRed(), colorFlash3.getGreen(), colorFlash3.getRed());
            else
                // ���� ������ 1-� ����� ����, �� �������� ������ ������� ������ � ����������� ����
                font.setColor(128, 255, 128);

            font.drawString(320, 200, (unsigned char*)"�������� � ����\n"); 

        } else

        if (timer.getTime() - gameView.getTimeNewPhase() > ASK_CONTINUE_DELAY)
        {
            colorFlash1.process(timer.getTime());

            font.setColor(colorFlash1.getRed(), colorFlash1.getGreen(), colorFlash1.getBlue());

            font.drawString(243, 30, (unsigned char*)"��� ������ ���� ����� <Enter>\n");
                       
            // ������������� � ��������� �������� ������ ����� ���� (� ��������� ������� �� ������ �����������)
            gameView.setState(READY_TO_START);
        }

    } // end of ����������� ���������� ������ ����
    
    // ����� � ������������ ������ �������� ������ ��� ���� 
    if (gameView.getState() == SHOW_LEVEL_NUMBER)
    {
        float time = timer.getTime() - gameView.getTimeNewPhase();
        
        if (time > PAUSE_BEFORE_SHOW_LEVEL && time < TIME_FOR_SHOW_LEVEL_NUMBER)
        {
            font.setColor(128, 255, 128);
            
            // ����������� ������ � ��������� (���������� �����) ������
            font.drawString(350, 330, (unsigned char*)level.stringName);

        } else
        if (time > TIME_FOR_SHOW_LEVEL_NUMBER)
        {
            // ������������� � ����� ���� 
            gameView.setState(PLAY);

            // ��������� ������� ������� ��� ������ ��������� ����� �������� ������������ ������
            // ������ �������� CEnemy ������ ������ 
            level.lastEntryTime = timer.getTime();
        }
    }

	////////////////////////////////////////////////////
	// process effect list [change frame, position, etc]
    k = 0; // ������ ������� � ��������� effectList 
    for (i = 0; i < effectList.getElementsNumber(); i++)
    {
        tempEffect = effectList.getObject(k);

        if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER || 
            gameView.getState() == THE_END)
            tempEffect.processEffect(dt);

        // ��������� �������� 
        if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER
            || gameView.getState() == PAUSE || gameView.getState() == ASK_TO_ABORT || gameView.getState() == THE_END)
        {
            tempEffect.draw();
        }

        effectList.reloadObject(k, tempEffect);

        // delete effect if it is not active
        if (tempEffect.isAlive() == false)
            effectList.removeObject(i);
        else
            k++; // ��������� ������� ��� ������� � ���������� �������� effectList 
    }


    // ���������� � ��������� �������� FPS � ���������� ��������� ������� ����� 
    if (gameView.getState() == PLAY || gameView.getState() == LIFE_OVER || gameView.getState() == SHOW_GAME_OVER
        || gameView.getState() == PAUSE || gameView.getState() == ASK_TO_ABORT || gameView.getState() == THE_END)
    {
        // �������� ����������� �������� FPS ��� � �������
        if ((timer.getTime() - timer.getLastTime()) >= 1.0)
        {
            timer.setLastTime(timer.getTime());
            fps = timer.getFps();

            timer.clearFps();
        }
        else
        {
            timer.incrementFps();
        }

        // ���� ������ ������ <F> ��� ������ �������� FPS
        if (keys.isPressed(F_BUTTON) == true)
        {
            font.setColor(0, 162, 232);
            font.drawString(510, SCREEN_HEIGHT - 30, (unsigned char*)"FPS:\n");

            // !!! ���� �������� fps ������� �������
            if (fps >= 1000) fps = 999;

            // ����������� �������� ���������� ������, ��������� � ������� (3-��������� �����)
            number.draw(FPS_X, FPS_Y, 3, fps);
        }

        
        // ����������� ��������� ������� ����� (6-��������� �����)
        number.draw(SCORE_X, SCORE_Y, 6, player.getScore());

        // ����������� ���������� ������
        unsigned char status;
        for (i = 0; i < (unsigned int)player.getMaxLives(); i++)
        {
            if (((unsigned int)player.getMaxLives() - i) > (unsigned int)player.getLivesNumber())
                status = DEAD_TILE;
            else
                status = ALIVE_TILE;
            // ��������� ������� ���� ����� ���������� ����� (alive / dead) 
            lifeIndicator.draw((player.getMaxLives() - i - 1) * 17 + LIFE_BAR_X, LIFE_BAR_Y, status);
        }
        

        #ifdef OBJECT_DEBUG
        // ���������� ���������� �� �����������-����������� ��������
        // ������ ��� ���-��� ��������� ����� - ���-�� �������� ��������� ������ CEnemy
        // ��������� ������ ���� - ���-�� �������� ��������� CMissle
        // ��������� ������ ���� - ���-�� �������� ��������� CEffect 
        number.draw(733, SCREEN_HEIGHT -  72, 4, enemyList.getElementsNumber());
        number.draw(733, SCREEN_HEIGHT -  96, 4, missleList.getElementsNumber());
        number.draw(733, SCREEN_HEIGHT - 120, 4, effectList.getElementsNumber());
        #endif
        
    } // end of <������� ��������� � ������� ������> ������ ���� ������ ������� �������� 

    // ����������� ���������� �����
	if (gameView.getState() == PAUSE)
	{
        colorFlash2.process(timer.getTime());

		font.setColor(colorFlash2.getRed(), colorFlash2.getGreen(), colorFlash2.getBlue());
		font.drawString(372, 330, (unsigned char*)"�����\n");
	}

    // ����������� ���������� ������� �� ������� � ������� ���� 
    if (gameView.getState() == ASK_TO_ABORT)
    {
        menu[MENU_EXIT_TO_MAIN].drawSelector(); // ��������� ����� ��������� ��� ���� ������� ������� ������ � ������� ����

        colorFlash3.process(timer.getTime()); // ��������� ��������� �������

        if (menu[MENU_EXIT_TO_MAIN].getSelected() == 0)
            // ���� ������ 0-� ����� ����, �� �������� ������ ������� ������ � ���� �������
            font.setColor(colorFlash3.getRed(), colorFlash3.getGreen(), colorFlash3.getRed());
        else
            // ���� ������ 0-� ����� ����, �� �������� ������ ������� ������ � ����������� ����
            font.setColor(128, 255, 128);

        font.drawString(343, 330, (unsigned char*)"����������\n");

        if (menu[MENU_EXIT_TO_MAIN].getSelected() == 1)
            // ���� ������ 1-� ����� ����, �� �������� ������ ������� ������ � ���� �������
            font.setColor(colorFlash3.getRed(), colorFlash3.getGreen(), colorFlash3.getRed());
        else
            // ���� ������ 1-� ����� ����, �� �������� ������ ������� ������ � ����������� ����
            font.setColor(128, 255, 128);

        font.drawString(331, 290, (unsigned char*)"����� �� ����\n");

    }


    // ��������� � ����������� ��������� ����������� ����
    if (gameView.getState() == THE_END)
    {
        colorFlash3.process(timer.getTime());

        font.setColor(colorFlash3.getRed(), colorFlash3.getGreen(), colorFlash3.getBlue());
        font.drawString(300, 330, (unsigned char*)"������� ���������!!\n");
        font.drawString(327, 300, (unsigned char*)"���� ��������!\n");

        // ������ ����� �� ����� ���������� � ��������� THE_END 
        if ((timer.getTime() - gameView.getTimeNewPhase()) > TIME_SHOW_THE_END)
        {
            gameView.setState(MAIN_TITLE);
            // ���������� ������� ������� ��� ������ ��������� �� ������ ������ "��� ������..."
            gameView.setTimeNewPhase(timer.getTime());

            // �������� ������� �������� ������ (�����, ����� �������� ������ ������, ������������� ���������������)
            level.currentLevel = 0;
        }
    }


} // end of render


////////////////////////////
//	       Control        //
////////////////////////////
void CGame::controller(void)
{
	unsigned char playerDirection;
	float spd;

    //////////////////////////////
	// ��������� ������ ����������

    // ���� ������ ������ <�����>
    if (keys.isPressed(UP_ARROW_BUTTON))
    {
        playerDirection |= MOVE_UP;

        // ���� ��������� �� ������ ������� �� ����� �� ���� � ������� ����
        if (gameView.getState() == ASK_TO_ABORT)
            // ������� ���������� ����� ����, ���� � ������� ���������� ������ ������ ��������� ��������
            menu[MENU_EXIT_TO_MAIN].selectPrevItem(timer.getTime());

        // ���� ��������� �� ������ ������� �� ����� � �������
        if (gameView.getState() == ASK_TO_EXIT)
            // ������� ���������� ����� ����, ���� � ������� ���������� ������ ������ ��������� ��������
            menu[MENU_EXIT_TO_SYSTEM].selectPrevItem(timer.getTime());

    } else 
        playerDirection &= ~MOVE_UP;

    // ���� ������ ������ <����>
    if (keys.isPressed(DOWN_ARROW_BUTTON))
    {
        playerDirection |= MOVE_DOWN;

        // ���� ��������� �� ������ ������� �� ����� �� ���� � ������� ����
        if (gameView.getState() == ASK_TO_ABORT)
            // ������� ��������� ����� ����, ���� � ������� ���������� ������ ������ ��������� ��������
            menu[MENU_EXIT_TO_MAIN].selectNextItem(timer.getTime());

        // ���� ��������� �� ������ ������� �� ����� � �������
        if (gameView.getState() == ASK_TO_EXIT)
            // ������� ���������� ����� ����, ���� � ������� ���������� ������ ������ ��������� ��������
            menu[MENU_EXIT_TO_SYSTEM].selectNextItem(timer.getTime());

    } else 
        playerDirection &= ~MOVE_DOWN;

    // ���� ������ ������ <������>
	if (keys.isPressed(LEFT_ARROW_BUTTON)) playerDirection |= MOVE_LEFT;
	else playerDirection &= ~MOVE_LEFT;

    // ���� ������ ������ <�������>
	if (keys.isPressed(RIGHT_ARROW_BUTTON)) playerDirection |= MOVE_RIGHT;
	else playerDirection &= ~MOVE_RIGHT;

    // ��������� ����������� �������� ������ � ����������� �� ������� ������
	player.setDirection(playerDirection);


    // �������� ������� ������ <ENTER>
    if (keys.isPressed(ENTER_BUTTON))
    {
        switch (gameView.getState())
        {

        case ASK_TO_ABORT:

            if (menu[MENU_EXIT_TO_MAIN].getSelected() == 0)     // ������ ����� "������� � ����"
            {
                gameView.setState(PLAY);
                // ����� �������� ��������� ����� �� ������, ���� ������������ � ����, ������������ �� �����
                exitControl.setPause(false);

                // ��� ����������� � ���� �� ��������� ������� �� ����� � ������� ����� (���� - ����� �� �����), ������������ 
                // �������� �����������, ����� ��������������� � ������� ���������� CEnemy ��������� �������� fireStartPeriod, 
                // ����������� ������ CEnemy::correctLastFireTime() ��� �����, ����� ����� ����� �� ����������� ��� ������� 
                // ��������� ����� ��������� �����������
                CEnemy temp;
                for (unsigned int i = 0; i < enemyList.getElementsNumber(); i++)
                {
                    temp = enemyList.getObject(i);
                    // ��������� ������� �������� - ��������� �� �������� �����
                    temp.correctLastFireTime(temp.getLastFireTime() + gameView.getPauseDuration());
                    enemyList.reloadObject(i, temp);
                }

                // �����, ��� ����������� � ���� �� ��������� ������� �� �����, ������������ ��������� �������� ������� ������� 
                // ��������� ��������� ������ ����������� �� ������ ������
                level.lastEntryTime += gameView.getPauseDuration();

                // ���������� � ����� "���������� �� �����" �� ����� ������� �� �����  ������� ��������������� � ������ ��������, 
                // ���������� �� ������� !!!
            }
            else
            if (menu[MENU_EXIT_TO_MAIN].getSelected() == 1)     // ������ ����� "����� � ������� ����"
            {
                gameView.setState(MAIN_TITLE);
                // ���������� ������� ������� ��� ������ ��������� �� ������ ������ "��� ������..."
                gameView.setTimeNewPhase(timer.getTime());

                // �������� ������� �������� ������ (�����, ����� �������� ������ ������, ������������� ���������������)
                level.currentLevel = 0;
                // ����� �������� ��������� �����, �� ������, ���� ��������� � �������� ������ �� ��������� �����
                exitControl.setPause(false);
            }

            break;

        case ASK_TO_EXIT:

            if (menu[MENU_EXIT_TO_SYSTEM].getSelected() == 1)     // ������ ����� "�������� � ����"
            {
                gameView.setState(MAIN_TITLE);
                gameView.setTimeNewPhase(timer.getTime());
            }
            else
            if (menu[MENU_EXIT_TO_SYSTEM].getSelected() == 0)     // ������ ����� "����� � �������"
                exitControl.setState(true); // ������� �� ����� � �������

            break;

        case READY_TO_START: // ���� ��������� � ��������� �������� ������ ����

            // ��������� ���������� ���-�� ������ ������, ��� ������ ����� ���� 
            player.setLives(INITIAL_PLAYER_LIVES);
            // ��������� ����� ������ 
            player.clearScore();

            // ��������� ��������� �������� ����� ��������� � ���������� ������
            gameView.prepareForPlayLevel(timer.getTime());
            // ������������� � ����������� ������ � ������� ������ 
            gameView.setState(SHOW_LEVEL_NUMBER);
            // ������������� ������� �������
            gameView.setTimeNewPhase(timer.getTime());

            break;

        case SHOW_GAME_OVER:
        case THE_END:

            gameView.setState(MAIN_TITLE);
            // ���������� ������� ������� ��� ������ ��������� �� ������ ������ "��� ������..."
            gameView.setTimeNewPhase(timer.getTime());

            // �������� ������� �������� ������ (�����, ����� �������� ������ ������, ������������� ���������������)
            level.currentLevel = 0;

            break;
        }
    }
    
    // ��������, ���� ������ ������ ������� � ��������� � ��������� ���� (PLAY)
	if (keys.isPressed(Z_BUTTON) && gameView.getState() == PLAY)
	{
		if (player.isReadyToFire(timer.getTime()) == true)
		{
			player.setLastMissleTime(timer.getTime());
		
			spd = missle[0].getSpeedValue();

			// ������� ������� "��������" ��������� �������
			missle[0].setSpeed(0, spd);
			missle[0].init(player.getX(), player.getY());
			missleList.addObject(missle[0]);
		}
	}


    // ��������� �������� ������� ������ <�����>
    // 
    // ���� ������ <�����> ������
    if (keys.isPressed(P_BUTTON) == true)
    {
        // ����� ������� � ������ �����
        keys.setPressed(P_BUTTON, false);

        // ���� ��������� � ��������� ����
        if (gameView.getState() == PLAY)
            // ���������� ����� �����
            gameView.setState(PAUSE);

        else
        // ���� ��������� � ��������� �����
        if (gameView.getState() == PAUSE)
        {
            // ���������� ��������� ���� (�����������)
            gameView.setState(PLAY);
            // ����� �������� ��������� �����, �.�. ������� �� ��������� ����� � ��������� ���� 
            exitControl.setPause(false);
          
            // ��� ������ �� ��������� �����, ������������ �������� �����������, ����� ��������������� � 
            // ������� ���������� CEnemy ��������� �������� fireStartPeriod, ����������� ������ CEnemy::correctLastFireTime()
            // ��� �����, ����� ����� ����� �� ����������� ��� ������� ��������� ����� ��������� �����������
            CEnemy temp;
            for (unsigned int i = 0; i < enemyList.getElementsNumber(); i++)
            {
                temp = enemyList.getObject(i);
                // ��������� ������� �������� - ��������� �� �������� �����
                temp.correctLastFireTime(temp.getLastFireTime() + gameView.getPauseDuration());
                enemyList.reloadObject(i, temp);
            }
            
            // ����� ��� ������ �� ��������� ����� ������������ ��������� �������� ������� ������� ��������� ��������� 
            // ������ ����������� �� ������ ������
            level.lastEntryTime += gameView.getPauseDuration();

            // ���������� � ����� ���������� �� ����� ������� ��������������� � ������ ��������, ���������� �� �������
            // !!!
        }
    } 


    // ���� ������ ������ <ESC>
    if (keys.isPressed(ESC_BUTTON) == true)
    {
        // ���������� ������� <ESC> � ������ ����������
        keys.setPressed(ESC_BUTTON, false);

        switch (gameView.getState())
        {
        // � ����� ������� ������ ����� ���������� ��������� ���� �� �����
        case PLAY:
        case PAUSE:
            // ��������� �������� ������� ������ ����� � ������ ����������, ����� �� ��������� ������ � ����� 
            // ��� �������� ������� ������ ������ ����
            //keys.setPressed(P_BUTTON, true);
            // ��������� ��������� �����
            gameView.setState(ASK_TO_ABORT);        // ������� � ������ ������� �� ����� � ������� ����
            menu[MENU_EXIT_TO_MAIN].setSelected(0); // ��������� ��������� �������� ������ ����

            break;

        case MAIN_TITLE:
        case READY_TO_START:

            gameView.setState(ASK_TO_EXIT);             // ������� � ������ ������� �� ����� � �������
            menu[MENU_EXIT_TO_SYSTEM].setSelected(0);   // ��������� ��������� �������� ������ ����

            break;

        case ASK_TO_EXIT:

            gameView.setState(READY_TO_START);

            break;

        case THE_END:

            gameView.setState(MAIN_TITLE);
            // ���������� ������� ������� ��� ������ ��������� �� ������ ������ "��� ������..."
            gameView.setTimeNewPhase(timer.getTime());

            // �������� ������� �������� ������ (�����, ����� �������� ������ ������, ������������� ���������������)
            level.currentLevel = 0;

            break;


        case ASK_TO_ABORT:

            gameView.setState(PLAY);

            // ��� ������ �� ��������� ������� ������ ����, ������������ �������� �����������, ����� ��������������� � 
            // ������� ���������� CEnemy ��������� �������� fireStartPeriod, ����������� ������ CEnemy::correctLastFireTime()
            // ��� �����, ����� ����� ����� �� ����������� ��� ������� ��������� ����� ��������� �����������
            CEnemy temp;
            for (unsigned int i = 0; i < enemyList.getElementsNumber(); i++)
            {
                temp = enemyList.getObject(i);
                // ��������� ������� �������� - ��������� �� �������� �����
                temp.correctLastFireTime(temp.getLastFireTime() + gameView.getPauseDuration());
                enemyList.reloadObject(i, temp);
            }

            // ����� ��� ������ �� ��������� ������� ������ ���� ������������ ��������� �������� ������� ������� ��������� ��������� 
            // ������ ����������� �� ������ ������
            level.lastEntryTime += gameView.getPauseDuration();

            // ���������� � ����� ���������� �� ����� � ������� ������� ������ ���� ������� ��������������� � ������ ��������, 
            // ���������� �� �������
            // !!!


            break;

        }

    }
}




