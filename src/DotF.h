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
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>

#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "Character.h"
#include "Robot.h"
#include "Button.h"


//-----------------------------------------------------------------
// Resolution
//-----------------------------------------------------------------
const int RES_W = 1024;
const int RES_H = 768;

//-----------------------------------------------------------------
// Custom types
//-----------------------------------------------------------------
typedef vector<vector<int>> Map;

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
vector<Robot*>	robots; // all robots
vector<Robot*>	inGameRobots; // robots that are selected and will be loaded into actual game
vector<int>		selectedRobotIndexes; // store indexes of robots that are selected in robots vector
int				playerCount; // single player or multiplayer
int				numEnemyBases;
Map				maps[8];

//-----------------------------------------------------------------
// Bitmaps / Sprites
//-----------------------------------------------------------------

// Menu
Bitmap  	*bmMenuBackground;
Bitmap  	*bmMenuTitle;
Bitmap  	*bmMenuStar;

// Game
Bitmap		*bmWall;
Bitmap		*bmEnemyBase;
Sprite		*walls[100];
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

//-----------------------------------------------------------------
// Menu functions
//-----------------------------------------------------------------
void	CreateButtons(HDC _hDC);
void	HandleMenuButtonClick(int _x, int _y);
void	HandleMenuButtonHover(int _x, int _y);
void	DrawRobotsOnMenu(HDC _hDC);

//-----------------------------------------------------------------
// Utility functions
//-----------------------------------------------------------------
LPWSTR			ConvertString(const std::string& instr);
queue<POINT>	FindPath(POINT _source, POINT _destination, Map _map);
