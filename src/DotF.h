//-----------------------------------------------------------------
// DotF Application
// C++ Header - DotF.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <vector>
#include <algorithm>
#include <queue> 
#include <stack>
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>

#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "WallSprite.h"
#include "Character.h"
#include "Robot.h"
#include "Button.h"
#include "Demon.h"
#include "DemonBase.h"
#include "WololoRobot.h"

//-----------------------------------------------------------------
// Resolution
//-----------------------------------------------------------------
const int RES_W = 1024;
const int RES_H = 768;

//-----------------------------------------------------------------
// Custom types
//-----------------------------------------------------------------

typedef vector<vector<int>> Map;

class Coordinate {
public:
	int x;
	int y;
	int dist;
	Coordinate* prev;
	POINT screenPos;

public:
	Coordinate(int _x, int _y) {
		x = _x; 
		y = _y;
		dist = 0;    
		prev = NULL;
		screenPos = { (x*32), (y*32) };
	}
	Coordinate(int _x, int _y, int _dist, Coordinate* _prev) {
		x = _x; 
		y = _y;
		dist = _dist; 
		prev = _prev;
		screenPos = { (x * 32), (y * 32) };
	}
};

//-----------------------------------------------------------------
// Scene Flags
//-----------------------------------------------------------------
enum Scene {
	MENU_MAIN = 0,
	MENU_SELECT_PLAYERS = 1,
	MENU_SELECT_ROBOTS = 2,
	GAME_PLAY = 3,
	GAME_PAUSE = 4
};

//-----------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------
HINSTANCE   hInstance;
GameEngine* game;
HDC         offScreenDC;
HBITMAP     offScreenBitmap;
Scene		currentScene;

//-----------------------------------------------------------------
// Game
//-----------------------------------------------------------------
vector<DemonBase*> demonBases;
vector<Robot*>	robots; // all robots
vector<Robot*>	inGameRobots; // robots that are selected and will be loaded into actual game
vector<int>		selectedRobotIndexes; // store indexes of robots that are selected in robots vector
int				playerCount; // single player or multiplayer
int				numEnemyBases;
Map				maps[8];
Map				currentMap;
int				totalPoint;
time_t			lastHealTime;//for general heal process
//-----------------------------------------------------------------
// Input delays
//-----------------------------------------------------------------
int		dlPlayer1Switch;
int		dlPlayer2Switch;
int				numSwitch; // counter for switching robots

//-----------------------------------------------------------------
// Bitmaps / Sprites
//-----------------------------------------------------------------

// Menu
Bitmap  	*bmMenuBackground;
Bitmap  	*bmMenuBackground2;
Bitmap  	*bmMenuTitle;
Bitmap  	*bmMenuStar;

// Game
Bitmap		*bmWall1, *bmWall2, *bmWall3;
Bitmap		*bmEnemyBase;
Bitmap		*bmBullet;
Bitmap		*bmDemonBullet;
Bitmap		*bmDemon;
Bitmap		*bmMonster;
Bitmap		*bmRobotMenu;
Bitmap		*bmExplosion;

Bitmap		*bmRobot1Walk;

Sprite		*spDemon;
WallSprite	*walls[100];
Sprite		*enemyBases[20];

//-----------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------
vector<Button*>	menuMainButtons, menuPlayersButtons, menuRobotsButtons;
Button			*btnPlay, *btnExit;
Button			*btn1Player, *btn2Player, *btnBackToMain;
Button			*btnReady, *btnBackToPlayers;


//-----------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// Game (play) functions
//-----------------------------------------------------------------

// Initializing
void	CreateRobots(HDC _hDC);
void	InitializeLevel();
Map		CreateMap(bool _firstLevel);

// Update
void	Player1Controls();
void	Player2Controls();
Robot*  GetPlayersRobot(int _player);
void	SwitchRobot(int _player);
void	Fire(Character *character);
void	UpdateCharacters();
void	UpdateBases();
void	UpdateObstacles();

Demon*	AddDemon(DemonBase* _base);
void	KillCharacter(Character* _character);

//-----------------------------------------------------------------
// Draw functions
//-----------------------------------------------------------------
void	DrawIndicators(HDC _hDC, Character* _character);
void	DrawRobotsOnMenu(HDC _hDC);
void	CreateButtons(HDC _hDC);

//-----------------------------------------------------------------
// Menu functions
//-----------------------------------------------------------------
void	HandleMenuButtonClick(int _x, int _y);
void	HandleMenuButtonHover(int _x, int _y);

//-----------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------
POINT			ScreenRectToArrayPoint(RECT _rect);
POINT			ScreenToArrayPoint(POINT _pt);
POINT			RectToPoint(RECT rect);
BOOL			CmpPoint(POINT p1, POINT p2);
void			PrintPath(stack<POINT> _path);
void			PrintPathOnScreen(HDC _hDC, stack<POINT> _path);
int				GetDistance(POINT p1, POINT p2);
LPWSTR			ConvertString(const std::string& instr);
POINT			CheckCollisionBeforeMoving(Character *robot, POINT velocity);
int				CoordinateDistance(Coordinate *c1, Coordinate *c2);
stack<POINT>	FindPathBFS(POINT _src, POINT _dst, Map _currentMap);
stack<POINT>	DemonRoam(Demon *_demon, POINT robotposition, POINT baselocation, Map currentmap);
stack <POINT>	FollowRobot(POINT src, POINT dst, Map _map);
void			CheckAbilities();
void			DrawAbilities(Robot *robot, int powerindex,HDC _hdc,int robotindex);
void			HealWithPoints(); // her 10 sn bir healt pointi 50 den düşük olanlara total point üzerinden can verir.