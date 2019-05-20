//-----------------------------------------------------------------
// Robot Pulsar2280 Object
// C++ Header - R_Pulsar2280.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <string>
#include <time.h>
#include "Robot.h"

//-----------------------------------------------------------------
// R_Pulsar2280 Class
//-----------------------------------------------------------------
class R_Pulsar2280 :public Robot
{
protected:
public:
	// Constructor(s)/Destructor
	R_Pulsar2280(string _name, string _description, Sprite * _sprite, Sprite * _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed);
	virtual ~R_Pulsar2280();
	// General Methods
	void UseAbility1();
	void UseAbility2(Robot* robot);
};
