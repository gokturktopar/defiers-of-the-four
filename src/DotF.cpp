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
	game = new GameEngine(_hInstance, (LPTSTR)TEXT("Defiers of the Four"),
		(LPTSTR)TEXT("Defiers of the Four"), (WORD)IDI_DOTF, (WORD)IDI_DOTF_SM, 1024, 768);
	if (game == NULL)
		return FALSE;

	// Set the frame rate
	game->SetFrameRate(30);

	// Store the instance handle
	hInstance = _hInstance;

	// Configure game variables
	currentScene = MENU_MAIN;
	playerCount = -1;

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

	// Set initial values
	numSwitch = 0;
	dlPlayer1Switch = 0;
	dlPlayer2Switch = 0;

	// Load bitmaps and sprites
	bmMenuBackground = new Bitmap(hDC, 1024, 768, RGB(22, 15, 28));
	bmMenuTitle = new Bitmap(hDC, IDB_MENU_TITLE, hInstance);
	bmMenuStar = new Bitmap(hDC, IDB_MENU_STAR, hInstance);

	bmWall = new Bitmap(hDC, IDB_WALL, hInstance);
	bmEnemyBase = new Bitmap(hDC, IDB_ENEMY_BASE, hInstance);

	// Create initial game elements
	CreateButtons(hDC);
	CreateRobots(hDC);
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

void GameActivate(HWND _hWindow) {}

void GameDeactivate(HWND _hWindow) {}

void GamePaint(HDC _hDC)
{
	switch (currentScene) {
	case MENU_MAIN:
		bmMenuBackground->Draw(_hDC, 0, 0);
		game->PrintText(_hDC, (LPTSTR)TEXT("Defiers of the Four"), 96, { 0, 150, 1024, 300 });
		for (auto &Button : menuMainButtons) {
			Button->Draw(_hDC);
		}
		break;
	case MENU_SELECT_PLAYERS:
		bmMenuBackground->Draw(_hDC, 0, 0);
		game->PrintText(_hDC, (LPTSTR)TEXT("Game Type"), 64, { 0, 150, 1024, 300 });
		for (auto &Button : menuPlayersButtons) {
			Button->Draw(_hDC);
		}
		break;
	case MENU_SELECT_ROBOTS:
		bmMenuBackground->Draw(_hDC, 0, 0);
		game->PrintText(_hDC, (LPTSTR)TEXT("Select Characters"), 64, { 0, 150, 1024, 300 });

		// Draw robots
		DrawRobotsOnMenu(_hDC);
		// Select menu buttons
		btnBackToPlayers->Draw(_hDC);
		if (selectedRobotIndexes.size() == playerCount * 2) {
			btnReady->Draw(_hDC);
		}
		break;
	case GAME_PLAY:
		bmMenuBackground->Draw(_hDC, 0, 0);
		for (int i = 0; i < 100; i++)
			walls[i]->Draw(_hDC);
		for (int i = 0; i < numEnemyBases; i++)
			enemyBases[i]->Draw(_hDC);
		game->DrawSprites(_hDC);

		for (auto &Robot : inGameRobots) {
			RECT bounds = Robot->GetSprite()->GetPosition();
			bounds.bottom = bounds.top;
			bounds.top -= 25;
			std::string identifier;
			if (Robot->GetControlStatus() == AI) identifier = "AI";
			else if (Robot->GetControlStatus() == PLAYER_2) identifier = "P2";
			else identifier = "P1";
			game->PrintText(_hDC, (LPCWSTR)ConvertString(identifier), 16, bounds);
		}
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

//-----------------------------------------------------------------
// Input
//-----------------------------------------------------------------
void HandleKeys()
{
	if (currentScene == GAME_PLAY) {
		Player1Controls();
		if (playerCount == 2) {
			Player2Controls();
		}
	}
}

void MouseButtonDown(int _x, int _y, BOOL _isLeftClick)
{
}

void MouseButtonUp(int _x, int _y, BOOL _isLeftClick)
{
	if (_isLeftClick) {
		if (currentScene == MENU_MAIN || currentScene == MENU_SELECT_PLAYERS || currentScene == MENU_SELECT_ROBOTS) {
			HandleMenuButtonClick(_x, _y);
		}

	}
}

void MouseMove(int _x, int _y)
{
	if (currentScene == MENU_MAIN || currentScene == MENU_SELECT_PLAYERS || currentScene == MENU_SELECT_ROBOTS) {
		// Hover effect for menu
		HandleMenuButtonHover(_x, _y);
	}
}

void HandleJoystick(JOYSTATE _joystickState) {}

BOOL SpriteCollision(Sprite* _spriteHitter, Sprite* _spriteHittee)
{
	// TODO; Stop the non - character
	POINT zeroVelocity = { 0, 0 };
	if (_spriteHitter->GetCharacter() == NULL) {
		_spriteHitter->SetVelocity(zeroVelocity);
	}
	else {
		_spriteHitter->SetVelocity(-_spriteHitter->GetVelocity().x, -_spriteHitter->GetVelocity().y);
	}
	return TRUE;
}

//-----------------------------------------------------------------
// Game
//-----------------------------------------------------------------
// Creates all robots in the game at the start.
void CreateRobots(HDC _hDC) {
	Bitmap *robotBitmap = new Bitmap(_hDC, IDB_MONSTER, hInstance); // 32x32
	Bitmap *menuBitmap = new Bitmap(_hDC, IDB_MENU_ROBOT, hInstance); // 100x100
	Sprite *robotSprite, *menuSprite;
	POINT pos;
	pos.x = 250;
	pos.y = 250;

	// 1 
	robotSprite = new Sprite(robotBitmap);
	menuSprite = new Sprite(menuBitmap);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot1 = new Robot({ "Robot 1" }, { "Robot 1 Desc" }, robotSprite, menuSprite, 100, 7, pos, AI, 10, 30);
	robotSprite->SetCharacter(robot1);

	// 2
	robotSprite = new Sprite(robotBitmap);
	menuSprite = new Sprite(menuBitmap);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot2 = new Robot({ "Robot 2" }, { "Robot 2 Desc" }, robotSprite, menuSprite, 100, 7, pos, AI, 10, 30);
	robotSprite->SetCharacter(robot2);

	// 3
	robotSprite = new Sprite(robotBitmap);
	menuSprite = new Sprite(menuBitmap);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot3 = new Robot({ "Robot 3" }, { "Robot 3 Desc" }, robotSprite, menuSprite, 100, 7, pos, AI, 10, 30);
	robotSprite->SetCharacter(robot3);

	// 4 
	robotSprite = new Sprite(robotBitmap);
	menuSprite = new Sprite(menuBitmap);
	robotSprite->SetSpriteType(ST_CHARACTER);
	Robot *robot4 = new Robot({ "Robot 4" }, { "Robot 4 Desc" }, robotSprite, menuSprite, 100, 7, pos, AI, 10, 30);
	robotSprite->SetCharacter(robot4);

	// Add to vector
	robots.push_back(robot1);
	robots.push_back(robot2);
	robots.push_back(robot3);
	robots.push_back(robot4);
}

// Handles Player 1's key controls, only in game play scene.
void Player1Controls() {
	Robot *robot = GetPlayersRobot(1);
	// Robot movement
	POINT velocity = { 0,0 };
	if (GetAsyncKeyState(VK_UP) < 0) {
		velocity.y = -robot->GetSpeed();
	}
	else if (GetAsyncKeyState(VK_DOWN) < 0) {
		velocity.y = robot->GetSpeed();
	}

	if (GetAsyncKeyState(VK_LEFT) < 0) {
		velocity.x = -robot->GetSpeed();
	}
	else if (GetAsyncKeyState(VK_RIGHT) < 0) {
		velocity.x = robot->GetSpeed();
	}

	// Slow down on diagonal, othersie diagonal moves are x2 faster than normal ones.
	if (velocity.x != 0 && velocity.y != 0) {
		velocity.x = round(((float)velocity.x) / 1.4);
		velocity.y = round(((float)velocity.y) / 1.4);
	}

	robot->GetSprite()->SetVelocity(velocity);

	// Switch robots
	if (++dlPlayer1Switch > 10 && GetAsyncKeyState(VK_NUMPAD0) < 0) {
		SwitchRobot(1);
		dlPlayer1Switch = 0;
	}

	// Fire

	// Abilities

	// Pause
}

// Handles Player 2's key controls, only in game play scene.
void Player2Controls() {
	Robot *robot = GetPlayersRobot(2);
	// Robot movement
	POINT velocity = { 0,0 };
	if (GetAsyncKeyState('W') < 0) {
		velocity.y = -robot->GetSpeed();
	}
	else if (GetAsyncKeyState('S') < 0) {
		velocity.y = robot->GetSpeed();
	}

	if (GetAsyncKeyState('A') < 0) {
		velocity.x = -robot->GetSpeed();
	}
	else if (GetAsyncKeyState('D') < 0) {
		velocity.x = robot->GetSpeed();
	}

	// Slow down on diagonal, othersie diagonal moves are x2 faster than normal ones.
	if (velocity.x != 0 && velocity.y != 0) {
		velocity.x = round(((float)velocity.x) / 1.4);
		velocity.y = round(((float)velocity.y) / 1.4);
	}
	robot->GetSprite()->SetVelocity(velocity);

	// Switch robot
	if (++dlPlayer2Switch > 10 && GetAsyncKeyState(VK_TAB) < 0) {
		SwitchRobot(2);
		dlPlayer2Switch = 0;
	}
}

// Returns the robot currently controlled by _player.
Robot* GetPlayersRobot(int _player) {
	switch (_player) {
	case 1:
		for (auto &Robot : inGameRobots) {
			if (Robot->GetControlStatus() == PLAYER_1) {
				return Robot;
			}
		}
		return NULL;
	case 2:
		for (auto &Robot : inGameRobots) {
			if (Robot->GetControlStatus() == PLAYER_2) {
				return Robot;
			}
		}
		return NULL;
	default:
		// This should not happen
		return NULL;
	}
}

// Switch to next AI robot for _player.
void SwitchRobot(int _player) {
	int setAI = -1;
	int setPlayer = -1;
	while (1) {
		if (inGameRobots.at(numSwitch)->GetControlStatus() == AI) {
			setPlayer = numSwitch;
		}
		else if (_player == 1 && inGameRobots.at(numSwitch)->GetControlStatus() == PLAYER_1) {
			setAI = numSwitch;
		}
		else if (_player == 2 && inGameRobots.at(numSwitch)->GetControlStatus() == PLAYER_2) {
			setAI = numSwitch;
		}

		numSwitch++;
		if (numSwitch == inGameRobots.size()) {
			numSwitch = 0;
		}

		if (setAI != -1 && setPlayer != -1) {
			inGameRobots.at(setAI)->SetControlStatus(AI);

			if (_player == 1) {
				inGameRobots.at(setPlayer)->SetControlStatus(PLAYER_1);
			}
			else {
				inGameRobots.at(setPlayer)->SetControlStatus(PLAYER_2);
			}
			return;
		}
	}
}

// Passes selected robot indexes to Robot instances.
// Assigns robots to players.
// Creates map.
void InitializeLevel() {
	// Get selected robots IDs, then find the instance
	// of these robots, and push it to vector which
	// will be used during gameplay to control robots.
	// Also add sprites to sprite manager.
	for (auto i : selectedRobotIndexes) {
		inGameRobots.push_back(robots[i]);
		robots[i]->GetSprite()->SetPosition(i*64 + 500, 700);
		robots[i]->GetSprite()->SetZOrder(10);
		game->AddSprite(robots[i]->GetSprite());
	}

	// Initially add robot control to each player.
	// Default value of control status is AI.
	inGameRobots[0]->SetControlStatus(PLAYER_1);
	if (playerCount == 2) {
		inGameRobots[1]->SetControlStatus(PLAYER_2);
	}

	// Create map
	for (size_t i = 0; i < 8; i++)
	{
		if (i == 0) {
			Map newmap = CreateMap(true);
			maps[i] = newmap;
		}
		else
		{
			Map newmap = CreateMap(false);
			maps[i] = newmap;
		}
	}

	int enemyBaseCount = 0;
	int wallCount = 0;

	// 0 -> Empty
	// 1 -> Enemy base
	// 2 -> Wall
	for (size_t i = 0; i < maps[0].size(); i++)
	{
		for (size_t j = 0; j < maps[0][0].size(); j++)
		{
			// left to right; which map, which row, which col.
			if (maps[0][i][j] == 1)
			{
				enemyBases[enemyBaseCount] = new Sprite(bmEnemyBase);
				game->AddSprite(enemyBases[enemyBaseCount]);
				enemyBases[enemyBaseCount]->SetPosition(j * 32, i * 32);
				numEnemyBases++;
				enemyBaseCount++;
			}
			if (maps[0][i][j] == 2)
			{
				walls[wallCount] = new Sprite(bmWall);
				game->AddSprite(walls[wallCount]);
				walls[wallCount]->SetPosition(j * 32, i * 32);
				wallCount++;
			}
		}
	}

}

// Handles map generation.
// Returns a 2D int vector, numbers representing game objects.
Map CreateMap(bool _firstLevel) 
{
	// Draw the sprites in the sprite vector
	int number_of_wall = 100;
	int wall_lenght;// yan yana gelebilecek duvar sayısı
	int number_of_enemy_center;
	int wall_width = 32;
	int wall_height = 32;
	int enemy_center_width = 32;
	int enemy_center_height = 32;
	const int screen_width = 1024; // column =x  
	const int screen_height = 700; // row =y
	srand(time(NULL));
	number_of_enemy_center = rand() % 10 + 3;     //range 1 to 20
	int map_row = 24;
	int map_col = 32;
	vector< vector<int> > map(map_row, vector<int>(map_col, 0));

	if (_firstLevel)
	{
		for (size_t i = 20; i < map_row; i++)
		{
			for (size_t j = 0; j < map_col; j++)
			{
				map[i][j] = 0;
			}
		}
		map_row = 20;
	}
	else
	{
		map_row = 24;
	}

	vector<vector<int>> wall_positions(number_of_wall, vector<int>(2, 0)); // duvarların ve enemy centerların başlangıç noktalarını sol üst köşe referans noktası olarak tutar. 
	vector<vector<int>> enemy_center_positions(number_of_enemy_center, vector<int>(2, 0)); // duvarların ve enemy centerların başlangıç noktalarını sol üst köşe referans noktası olarak tutar. 

	// 0: empty
	// 1: enemy base
	// 2: wall
	int array_counter = 0;
	while (number_of_enemy_center > 0) {
		int row_a[] = { 4,11,18 };
		int col_a[] = { 4,11,18,25 };
		srand(time(NULL));
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng3(rd()); // seed the generator
		std::uniform_int_distribution<> distr3(0, 3);
		int col = distr3(eng3); //rand() % 5; //4,11,17,23,29
		col = col_a[col];
		srand(time(NULL));
		std::random_device rd2; // obtain a random number from hardware
		std::mt19937 eng4(rd2()); // seed the generator
		std::uniform_int_distribution<> distr4(0, 2);
		int row = distr4(eng4); //4,11,17,23
		row = row_a[row];
		bool avaliable_for_enemyC = false;

		while (!avaliable_for_enemyC)
		{
			if (map[row][col] != 0)
			{
				std::random_device rd; // obtain a random number from hardware
				std::mt19937 eng3(rd()); // seed the generator
				std::uniform_int_distribution<> distr3(0, 3);
				col = distr3(eng3); //rand() % 5; //4,11,17,23,29
				col = col_a[col];
				srand(time(NULL));
				std::random_device rd2; // obtain a random number from hardware
				std::mt19937 eng4(rd2()); // seed the generator
				std::uniform_int_distribution<> distr4(0, 2);
				row = distr4(eng4);
				row = row_a[row];
				break;
			}
			else {
				avaliable_for_enemyC = true;
				enemy_center_positions[array_counter][0] = row * 32;
				enemy_center_positions[array_counter][1] = col * enemy_center_height;
				array_counter++;
				number_of_enemy_center--;
				map[row][col] = 1;
			}
		}
	}

	array_counter = 0;
	while (number_of_wall > 0) {
		wall_lenght = rand() % 5 + 1;//range 1 to 5
		int wall_position;// = rand() % 4;
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 eng3(rd()); // seed the generator
		std::uniform_int_distribution<> distr3(0, 100);
		wall_position = distr3(eng3);
		if (number_of_wall - wall_lenght < 0) {
			wall_lenght = number_of_wall;
			number_of_wall -= wall_lenght;
		}
		else {
			number_of_wall = number_of_wall - wall_lenght;
		}
		if (wall_position < 65) //right
		{
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 eng3(rd()); // seed the generator
			std::uniform_int_distribution<> distr3(0, map_col - 1);
			int col1 = distr3(eng3);

			srand(time(NULL));
			std::random_device rd2; // obtain a random number from hardware
			std::mt19937 eng4(rd2());
			std::uniform_int_distribution<> distr4(0, map_row - 1);
			int row1 = distr4(eng4);

			bool avaliable_for_wall = false;

			while (!avaliable_for_wall)
			{
				if ((wall_lenght + col1) < map_col)
				{
					avaliable_for_wall = true;
					for (int j = col1; j < (col1 + wall_lenght) && avaliable_for_wall; j++)
					{
						if (map[row1][j] != 0)
						{
							std::mt19937 eng3(rd()); // seed the generator
							std::uniform_int_distribution<> distr3(0, map_col - 1);
							col1 = distr3(eng3);
							srand(time(NULL));
							std::random_device rd2; // obtain a random number from hardware
							std::mt19937 eng4(rd2());
							std::uniform_int_distribution<> distr4(0, map_row - 1);
							row1 = distr4(eng4);
							avaliable_for_wall = false;
							break;
						}
					}
				}
				else {
					std::mt19937 eng3(rd()); // seed the generator
					std::uniform_int_distribution<> distr3(0, map_col - 1);
					col1 = distr3(eng3);
					srand(time(NULL));
					std::random_device rd2; // obtain a random number from hardware
					std::mt19937 eng4(rd2());
					std::uniform_int_distribution<> distr4(0, map_row - 1);
					row1 = distr4(eng4);
				}
			}
			for (size_t i = 0; i < wall_lenght; i++)
			{
				wall_positions[array_counter][0] = row1 * wall_width;
				wall_positions[array_counter][1] = (col1 + i)*wall_height;
				array_counter++;
			}
			for (int j = col1; j < (col1 + wall_lenght); j++)
			{
				map[row1][j] = 2;
			}
		}
		else if (wall_position >= 65) // up
		{
			std::mt19937 eng3(rd()); // seed the generator
			std::uniform_int_distribution<> distr3(0, map_col - 1);
			int col2 = distr3(eng3);
			srand(time(NULL));
			std::random_device rd2; // obtain a random number from hardware
			std::mt19937 eng4(rd2());
			std::uniform_int_distribution<> distr4(0, 20);
			int row2 = distr4(eng4);
			bool avaliable_for_wall = false;

			while (!avaliable_for_wall)
			{
				if ((wall_lenght + row2) < map_row)
				{
					avaliable_for_wall = true;
					for (int j = row2; j < (row2 + wall_lenght) && avaliable_for_wall; j++)
					{
						if (map[j][col2] != 0)
						{
							std::mt19937 eng3(rd()); // seed the generator
							std::uniform_int_distribution<> distr3(0, map_col - 1);
							col2 = distr3(eng3);
							srand(time(NULL));
							std::random_device rd2; // obtain a random number from hardware
							std::mt19937 eng4(rd2());
							std::uniform_int_distribution<> distr4(0, map_row - 1);
							row2 = distr4(eng4);
							avaliable_for_wall = false;
							break;
						}
					}
				}
				else {
					std::mt19937 eng3(rd()); // seed the generator
					std::uniform_int_distribution<> distr3(0, map_col - 1);
					col2 = distr3(eng3);
					srand(time(NULL));
					std::random_device rd2; // obtain a random number from hardware
					std::mt19937 eng4(rd2());
					std::uniform_int_distribution<> distr4(0, map_row - 1);
					row2 = distr4(eng4);
				}
			}
			for (size_t i = 0; i < wall_lenght; i++)
			{
				wall_positions[array_counter][0] = (row2 + i) * wall_width;
				wall_positions[array_counter][1] = col2 * wall_height;
				array_counter++;
			}
			for (int j = row2; j < (row2 + wall_lenght); j++)
			{
				map[j][col2] = 2;
			}
		}
	}

	return map;
}

//-----------------------------------------------------------------
// Menu
//-----------------------------------------------------------------
void CreateButtons(HDC _hDC) 
{
	// Main menu buttons 
	btnPlay = new Button(_hDC, (LPTSTR)TEXT("Play"), (RES_W / 2) - (BTN_WIDTH / 2), 400);
	menuMainButtons.push_back(btnPlay);
	btnExit = new Button(_hDC, (LPTSTR)TEXT("Exit"), (RES_W / 2) - (BTN_WIDTH / 2), 450);
	menuMainButtons.push_back(btnExit);

	// Play menu buttons 
	btn1Player = new Button(_hDC, (LPTSTR)TEXT("1 Player"), (RES_W / 2) - (BTN_WIDTH / 2), 400);
	menuPlayersButtons.push_back(btn1Player);
	btn2Player = new Button(_hDC, (LPTSTR)TEXT("2 Player"), (RES_W / 2) - (BTN_WIDTH / 2), 450);
	menuPlayersButtons.push_back(btn2Player);
	btnBackToMain = new Button(_hDC, (LPTSTR)TEXT("Back"), (RES_W / 2) - (BTN_WIDTH / 2), 500);
	menuPlayersButtons.push_back(btnBackToMain);

	// Character select buttons
	btnReady = new Button(_hDC, (LPTSTR)TEXT("Begin"), (RES_W / 2) - (BTN_WIDTH / 2), 650);
	menuRobotsButtons.push_back(btnReady);
	btnBackToPlayers = new Button(_hDC, (LPTSTR)TEXT("Back"), (RES_W / 2) - (BTN_WIDTH / 2), 700);
	menuRobotsButtons.push_back(btnBackToPlayers);
}

// Handles click on buttons in menu.
// Scene changes and character selections are also here.
void HandleMenuButtonClick(int _x, int _y) 
{
	int i = 0; // to retrieve which robot is clicked

	switch (currentScene) {
	case MENU_MAIN:
		// Play clicked
		if (btnPlay->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = MENU_SELECT_PLAYERS;
		}

		// Exit clicked
		if (btnExit->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK_BACK, hInstance, SND_ASYNC | SND_RESOURCE);
			GameEnd();
		}
		break;
	case MENU_SELECT_PLAYERS:
		// Back to main menu clicked
		if (btnBackToMain->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK_BACK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = MENU_MAIN;
		}

		// 1 - Player selected
		if (btn1Player->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			playerCount = 1;
			currentScene = MENU_SELECT_ROBOTS;
		}

		// 2 - Player selected
		if (btn2Player->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			playerCount = 2;
			currentScene = MENU_SELECT_ROBOTS;
		}
		break;
	case MENU_SELECT_ROBOTS:
		// Button click - Ready
		if (btnReady->GetSprite()->IsPointInside(_x, _y) && playerCount * 2 == selectedRobotIndexes.size()) {
			InitializeLevel();
			PlaySound((LPCWSTR)IDW_MENU_CLICK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = GAME_PLAY;
		}

		// Button click - Back
		if (btnBackToPlayers->GetSprite()->IsPointInside(_x, _y)) {
			PlaySound((LPCWSTR)IDW_MENU_CLICK_BACK, hInstance, SND_ASYNC | SND_RESOURCE);
			currentScene = MENU_SELECT_PLAYERS;
		}

		// Robot select
		for (auto &Robot : robots) {
			if (Robot->GetMenuSprite()->IsPointInside(_x, _y)) {
				bool robotIsSelected = std::find(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i) != selectedRobotIndexes.end();

				// Add to selected if not selected, erase otherwise
				if (!robotIsSelected) {
					PlaySound((LPCWSTR)IDW_MENU_SELECT, hInstance, SND_ASYNC | SND_RESOURCE);
					selectedRobotIndexes.push_back(i);
				}
				else {
					selectedRobotIndexes.erase(std::remove(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i), selectedRobotIndexes.end());
				}
			}
			i++;
		}
		break;
	default:
		break;
	}
}

// Handles hovers on buttons in menu.
// It is visual rather than functional.
void HandleMenuButtonHover(int _x, int _y) 
{
	switch (currentScene) {
	case MENU_MAIN:
		for (auto &Button : menuMainButtons)
		{
			if (Button->GetSprite()->IsPointInside(_x, _y)) {
				Button->SetHover(true);
			}
			else {
				Button->SetHover(false);
			}
		}
		break;
	case MENU_SELECT_PLAYERS:
		for (auto &Button : menuPlayersButtons)
		{
			if (Button->GetSprite()->IsPointInside(_x, _y)) {
				Button->SetHover(true);
			}
			else {
				Button->SetHover(false);
			}
		}
		break;
	case MENU_SELECT_ROBOTS:
		for (auto &Button : menuRobotsButtons)
		{
			if (Button->GetSprite()->IsPointInside(_x, _y)) {
				Button->SetHover(true);
			}
			else {
				Button->SetHover(false);
			}
		}

		for (auto &Robot : robots) {
			if (Robot->GetMenuSprite()->IsPointInside(_x, _y)) {
				Robot->SetMenuHover(true);
			}
			else {
				Robot->SetMenuHover(false);
			}
		}
		break;
	default:
		break;
	}
}

// Draw robots on the select menu with their menu sprites.
// Displays name and descriptions on hover.
void DrawRobotsOnMenu(HDC _hDC) 
{
	// 100 x 100 robot icons
	int iconWidth = 100;
	int offset = iconWidth + iconWidth;

	// Calculate to center robot icons
	POINT start;
	start.x = (RES_W / 2) - (iconWidth / 2) - ((robots.size() - 1) * iconWidth);
	start.y = 300;

	int i = 0; // to see which robot is clicked
	for (auto &Robot : robots) {
		Robot->GetMenuSprite()->SetPosition(start.x + (i*offset), start.y); // set new position
		bool robotIsSelected = std::find(selectedRobotIndexes.begin(), selectedRobotIndexes.end(), i) != selectedRobotIndexes.end(); // check if robot is selected

		/*
		// If robot is hovered:
		//		not selected -> white focus
		//		selected	 -> red focus
		// If not hovered:
		//		not selected -> nothing
		//		selected	 -> green focus
		*/
		if (Robot->GetMenuHover()) {
			game->PrintText(_hDC, (LPCWSTR)ConvertString(Robot->GetName()), 36, { 0, 500, 1024, 550 });
			game->PrintText(_hDC, (LPCWSTR)ConvertString(Robot->GetDescription()), 36, { 224, 550, 800, 700 });

			HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
			if (robotIsSelected) {
				SelectObject(_hDC, hRedBrush);
			}

			Rectangle(_hDC, Robot->GetMenuSprite()->GetPosition().left - 5, Robot->GetMenuSprite()->GetPosition().top - 5,
				Robot->GetMenuSprite()->GetPosition().right + 20, Robot->GetMenuSprite()->GetPosition().bottom + 20);
			DeleteObject(hRedBrush);
		}
		else if (robotIsSelected) {
			HBRUSH hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
			SelectObject(_hDC, hGreenBrush);

			Rectangle(_hDC, Robot->GetMenuSprite()->GetPosition().left - 5, Robot->GetMenuSprite()->GetPosition().top - 5,
				Robot->GetMenuSprite()->GetPosition().right + 20, Robot->GetMenuSprite()->GetPosition().bottom + 20);
			DeleteObject(hGreenBrush);
		}
		Robot->GetMenuSprite()->Draw(_hDC);
		i++;
	}

	// Display how many robots more should be selected
	if (playerCount * 2 > selectedRobotIndexes.size()) {
		std::string remaining = std::to_string(playerCount * 2 - selectedRobotIndexes.size()) + " remaining";
		game->PrintText(_hDC, (LPCWSTR)ConvertString(remaining), 24, { 0, 250, 1024, 300 });
	}
}

//-----------------------------------------------------------------
// Utility
//-----------------------------------------------------------------
// Converts std::string to LPWSTR.
// Neccessary for DRAW TEXT.
LPWSTR ConvertString(const std::string& instr) 
{
	// Assumes std::string is encoded in the current Windows ANSI codepage
	int bufferlen = ::MultiByteToWideChar(CP_ACP, 0, instr.c_str(), instr.size(), NULL, 0);

	if (bufferlen == 0)
	{
		// Something went wrong. Perhaps, check GetLastError() and log.
		return 0;
	}

	// Allocate new LPWSTR - must deallocate it later
	LPWSTR widestr = new WCHAR[bufferlen + 1];

	::MultiByteToWideChar(CP_ACP, 0, instr.c_str(), instr.size(), widestr, bufferlen);

	// Ensure wide string is null terminated
	widestr[bufferlen] = 0;

	// Do something with widestr
	return widestr;
	//delete[] widestr;
}

