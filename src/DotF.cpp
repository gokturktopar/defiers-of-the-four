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
BOOL GameInitialize(HINSTANCE hInstance)
{
	// Create the game engine
	_pGame = new GameEngine(hInstance, (LPTSTR) TEXT("Defiers of the Four"),
		(LPTSTR) TEXT("Defiers of the Four"), (WORD) IDI_DOTF, (WORD) IDI_DOTF_SM, 1024, 768);
	if (_pGame == NULL)
		return FALSE;

	// Set the frame rate
	_pGame->SetFrameRate(30);

	// Store the instance handle
	_hInstance = hInstance;

	return TRUE;
}

void GameStart(HWND hWindow)
{
	// Seed the random number generator
	srand(GetTickCount());

	// Create the offscreen device context and bitmap
	_hOffscreenDC = CreateCompatibleDC(GetDC(hWindow));
	_hOffscreenBitmap = CreateCompatibleBitmap(GetDC(hWindow),
		_pGame->GetWidth(), _pGame->GetHeight());
	SelectObject(_hOffscreenDC, _hOffscreenBitmap);

	// Create and load the bitmaps
	HDC hDC = GetDC(hWindow);
}

void GameEnd()
{
	// Cleanup the offscreen device context and bitmap
	DeleteObject(_hOffscreenBitmap);
	DeleteDC(_hOffscreenDC);

	// Cleanup the sprites
	_pGame->CleanupSprites();

	// Cleanup the game engine
	delete _pGame;
}

void GameActivate(HWND hWindow)
{
}

void GameDeactivate(HWND hWindow)
{
}

void GamePaint(HDC hDC)
{
	// Draw the background


	// Draw the sprites
	_pGame->DrawSprites(hDC);
}

void GameCycle()
{
	// Update the sprites
	_pGame->UpdateSprites();

	// Obtain a device context for repainting the game
	HWND  hWindow = _pGame->GetWindow();
	HDC   hDC = GetDC(hWindow);

	// Paint the game to the offscreen device context
	GamePaint(_hOffscreenDC);

	// Blit the offscreen bitmap to the game screen
	BitBlt(hDC, 0, 0, _pGame->GetWidth(), _pGame->GetHeight(),
		_hOffscreenDC, 0, 0, SRCCOPY);

	// Cleanup
	ReleaseDC(hWindow, hDC);
}

void HandleKeys()
{
}

void MouseButtonDown(int x, int y, BOOL bLeft)
{

}

void MouseButtonUp(int x, int y, BOOL bLeft)
{
}

void MouseMove(int x, int y)
{

}

void HandleJoystick(JOYSTATE jsJoystickState)
{
}

BOOL SpriteCollision(Sprite* pSpriteHitter, Sprite* pSpriteHittee)
{
	// Swap the sprite velocities so that they appear to bounce
	POINT ptSwapVelocity = pSpriteHitter->GetVelocity();
	pSpriteHitter->SetVelocity(pSpriteHittee->GetVelocity());
	pSpriteHittee->SetVelocity(ptSwapVelocity);
	return TRUE;
}
