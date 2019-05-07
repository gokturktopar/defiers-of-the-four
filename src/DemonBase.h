//-----------------------------------------------------------------
// Bitmap Object
// C++ Header - DemonBase.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
using namespace std;
#include <windows.h>
#include <string>
#include <vector>
#include "Sprite.h"
#include "Robot.h"

#ifndef DEMON_H
#define DEMON_H
#endif

class Demon;

//-----------------------------------------------------------------
// Custom Data Types
//-----------------------------------------------------------------
/*
int x_bound_of_view_field = 6;
int y_bound_of_view_field = 6;*/

//-----------------------------------------------------------------
// DemonBase Class
//-----------------------------------------------------------------
class DemonBase
{
protected:

	// Member Variables
	Sprite* _demonBaseSprite;

	POINT mapPosition;
	int base_number;
	vector<Robot*> currentTargets;
	vector<Demon*> currentDemons;


	// Helper Methods
	void Free();

public:
	// Constructor(s)/Destructor
	DemonBase(POINT _mapPosition, int _base_number, Sprite* _demonBaseSprite);
	virtual ~DemonBase();

	// General Methods

	// Accessor Methods

	int GetBaseNumber() { return base_number; };
	void SetBaseNumber(int __base_number) { base_number = __base_number; };

	vector<Robot*> GetCurrentTargets() { return currentTargets; };
	void SetCurrentTargets(vector<Robot*> _currentTargets) { currentTargets = _currentTargets; };

	vector<Demon*> GetCurrentDemons() { return currentDemons; };
	void SetCurrentDemons(vector<Demon*> _currentDemons) { currentDemons = _currentDemons; };

	POINT GetMapPosition() { return mapPosition; };
	void SetMapPosition(POINT __mapPosition) { mapPosition = __mapPosition; };

};