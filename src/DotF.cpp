//-----------------------------------------------------------------
// Defiers of the Four Application
// C++ Source - DotF.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "DotF.h"


//-----------------------------------------------------------------
// Game Engine Functions
//-----------------------------------------------------------------
BOOL GameInitialize(HINSTANCE _hInstance)
{
	// Create the game engine
	game = new GameEngine(_hInstance, (LPTSTR) TEXT("Defiers of the Four"),
		(LPTSTR) TEXT("Defiers of the Four"), (WORD) IDI_DOTF, (WORD) IDI_DOTF_SM, 1024, 768);
	if (game == NULL)
		return FALSE;

	// Set the frame rate
	game->SetFrameRate(30);

	// Store the instance handle
	hInstance = _hInstance;

	// Set scene
	currentScene = MAIN_MENU;

	// Configure game variables
	characters = new Character*[4];

	return TRUE;
}

void GameStart(HWND _hWindow)
{
	// Seed the random number generator
	srand(GetTickCount());

	// Create the offscreen device context and bitmap
	offScreenDC = CreateCompatibleDC(GetDC(_hWindow));
	offScreenBitmap = CreateCompatibleBitmap(GetDC(_hWindow),
		game->GetWidth(), game->GetHeight());
	SelectObject(offScreenDC, offScreenBitmap);

	// Device context
	HDC hDC = GetDC(_hWindow);

	// Menu bitmaps and sprites
	bmMenuBackground = new Bitmap(hDC, 1024, 768, RGB(22, 15, 28));
	bmMenuTitle = new Bitmap(hDC, IDB_MENU_TITLE, hInstance);
	bmMenuStar = new Bitmap(hDC, IDB_MENU_STAR, hInstance);

	RECT starBounds = { 0, 0, 1024, 768 };
	POINT p;
	for (size_t i = 0; i < 32; i++) {
		sprMenuStars[i] = new Sprite(bmMenuStar, starBounds, BA_WRAP);
		sprMenuStars[i]->SetVelocity(4, 4);
		sprMenuStars[i]->SetPosition((i/4)*(1024/4), (i%8)*(768/8));
		game->AddSprite(sprMenuStars[i]);
	}
	
	// Load characters
	
	p.x = 0;
	p.y = 0;

	/*
	Bitmap *bmSheep = new Bitmap(hDC, IDB_SHEEP, hInstance);
	RECT rcBounds = { 0, 0, 600, 400 };
	Sprite *spr = new Sprite(bmSheep, rcBounds, BA_WRAP);
	spr->SetPosition(500, 300);
	game->AddSprite(spr);
	int cooldowns[2] = { 1, 5 };
	char name[20] = "RoboCap";
	characters[0] = new R_RoboCap(spr, p, p, 100, 6, name, cooldowns);*/
}

void GameEnd()
{
	// Cleanup the offscreen device context and bitmap
	DeleteObject(offScreenBitmap);
	DeleteDC(offScreenDC);

	// Cleanup the sprites
	game->CleanupSprites();

	// Cleanup the game engine
	delete game;
}

void GameActivate(HWND _hWindow)
{
}

void GameDeactivate(HWND _hWindow)
{
}

void GamePaint(HDC _hDC)
{
	switch (currentScene) {
		case MAIN_MENU:
			bmMenuBackground->Draw(_hDC, 0, 0);
			//game->DrawSprites(_hDC);
			bmMenuTitle->Draw(_hDC, 280, 150, 1, RGB(255, 0, 255));
			break;
		case PLAY_MENU:
			break;
		case SELECT_MENU:
			break;
		case GAME:
			// Draw the background

			// Draw the sprites
			game->DrawSprites(_hDC);
			break;
		case GAME_PAUSE:
			break;
		default:
			break;
	}
}

void GameCycle()
{
	// Update the sprites
	game->UpdateSprites();

	// Obtain a device context for repainting the game
	HWND  hWindow = game->GetWindow();
	HDC   hDC = GetDC(hWindow);

	// Paint the game to the offscreen device context
	GamePaint(offScreenDC);

	// Blit the offscreen bitmap to the game screen
	BitBlt(hDC, 0, 0, game->GetWidth(), game->GetHeight(),
		offScreenDC, 0, 0, SRCCOPY);

	// Cleanup
	ReleaseDC(hWindow, hDC);
}

void HandleKeys()
{
	POINT speed;
	speed.x = 0;
	speed.y = 0;

	/*if (GetAsyncKeyState('W') < 0) {
		speed.y = -characters[0]->GetSpeed();
	}
	else if (GetAsyncKeyState('S') < 0) {
		speed.y = characters[0]->GetSpeed();
	}

	if (GetAsyncKeyState('D') < 0) {
		speed.x = characters[0]->GetSpeed();
	}
	else if (GetAsyncKeyState('A') < 0) {
		speed.x = -characters[0]->GetSpeed();
	}
	
	POINT normalizedSpeed;
	normalizedSpeed = NormalizeSpeed(speed.x, speed.y);
	
	characters[0]->GetSprite()->SetVelocity(normalizedSpeed.x, normalizedSpeed.y);*/
}

POINT NormalizeSpeed(int _x, int _y) {
	POINT normalizedSpeed;
	normalizedSpeed.x = _x;
	normalizedSpeed.y = _y;

	if (_x != 0 && _y != 0) {
		normalizedSpeed.x = round(_x / 2);
		normalizedSpeed.y = round(_y / 2);
	}

	return normalizedSpeed;
}

void MouseButtonDown(int _x, int _y, BOOL _isLeftClik)
{

}

void MouseButtonUp(int _x, int _y, BOOL _isLeftClik)
{
}

void MouseMove(int _x, int _y)
{

}

void HandleJoystick(JOYSTATE _joystickState)
{
}

BOOL SpriteCollision(Sprite* _spriteHitter, Sprite* _spriteHittee)
{
	// Swap the sprite velocities so that they appear to bounce
	POINT ptSwapVelocity = _spriteHitter->GetVelocity();
	_spriteHitter->SetVelocity(_spriteHittee->GetVelocity());
	_spriteHittee->SetVelocity(ptSwapVelocity);
	return TRUE;
}
