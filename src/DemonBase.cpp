//-----------------------------------------------------------------
// Demon Object
// C++ Source - Demon.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "DemonBase.h"
#include "Sprite.h"

//-----------------------------------------------------------------
// DemonBase Constructor(s)/Destructor
//-----------------------------------------------------------------
DemonBase::DemonBase(POINT _mapPosition, int _base_number, Sprite* _base_sprite)
{
	mapPosition = _mapPosition;
	base_number = _base_number;

}

DemonBase::~DemonBase()
{
}