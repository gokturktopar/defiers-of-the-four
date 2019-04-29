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
#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "Character.h"
#include "R_RoboCap.h"
#include "Button.h"


//-----------------------------------------------------------------
// Resolution
//-----------------------------------------------------------------
const int RES_W = 1024;
const int RES_H = 768;

//-----------------------------------------------------------------
// Scene Flags
//-----------------------------------------------------------------
enum SCENE {
	MAIN_MENU = 0,
	PLAY_MENU = 1,
	SELECT_MENU = 2,
	GAME = 3,
	GAME_PAUSE = 4
};

//-----------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------
HINSTANCE   hInstance;
GameEngine* game;
HDC         offScreenDC;
HBITMAP     offScreenBitmap;
SCENE		currentScene;

//-----------------------------------------------------------------
// Game Variables
//-----------------------------------------------------------------
Character	**characters;
int			playerNum;

//-----------------------------------------------------------------
// Menu Bitmaps / Sprites
//-----------------------------------------------------------------
Bitmap  	*bmMenuBackground;
Bitmap  	*bmMenuTitle;
Bitmap  	*bmMenuStar;

//-----------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------
vector<Button*>	mainMenuButtons, playMenuButtons, characterMenuButtons;
Button			*btnPlay, *btnExit;
Button			*btn1Player, *btn2Player, *btnBackToMain;

//-----------------------------------------------------------------
// Menu - Character Select
//-----------------------------------------------------------------
enum SELECT_STATUS {
	DEFAULT = 0,
	FOCUSED = 1,
	SELECTED = 2
};

struct characters_t {
	Bitmap*			image;
	const char*		name;
	const char*		description;
	SELECT_STATUS	status;
};

characters_t*		selectCharacters[2];
int					currentCharacter = 0;


//-----------------------------------------------------------------
// Functions
//-----------------------------------------------------------------
void	HandleMenuButtonClick(int _x, int _y);
void	HandleMenuButtonHover(int _x, int _y);
void	SelectMenuDrawCharacters(HDC _hDC);