//-----------------------------------------------------------------
// R_Captain Object
// C++ Source - R_Captain.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "R_Captain.h"

//-----------------------------------------------------------------
// R_Captain Constructor(s)/Destructor
//-----------------------------------------------------------------

R_Captain::R_Captain(std::string _name, std::string _description, Sprite * _sprite, Sprite * _menuSprite, int _healthPoint, int _speed, POINT _mapPosition, int _fireSpeed)
	:Robot(_name, _description, _sprite, _menuSprite, _healthPoint, _speed, _mapPosition, _fireSpeed, CS_AI, false)
{
	name = { "Captain" };
	description = { "Leads the robots to battle by boosting their stats.\nAbility 1: Passive. Boosts armor and max health of the allies by 20%."};
	robotType = R_CAPTAIN;
	stats.armor = 5;
	// Set up first ability
	time_t now;
	time(&now);

	// Ability 1
	// Passive Aura. Always active. Boost nearby robots' stats.
	abilities[0].cooldown = -1;
	abilities[0].duration = -1;
	abilities[0].name = { "Bolster Courage" };
	abilities[0].ready = false;
	abilities[0].active = true;
	abilities[0].usedTime = time(0);

	// Ability 2
	// Active. Boost self stats for a duration.
	abilities[1].cooldown = 15;
	abilities[1].duration = 5;
	abilities[1].name = { "Unbreakable" };
	abilities[1].ready = true;
	abilities[1].active = false;
	abilities[1].usedTime = now;
}

R_Captain::~R_Captain()
{
}


//-----------------------------------------------------------------
// Character General Methods
//-----------------------------------------------------------------
void R_Captain::UseAbility1() {
	// passive
}

void R_Captain::UseAbility2() {
	if (abilities[1].ready == false) return;
	time_t now;
	time(&now);
	abilities[1].ready = false;
	abilities[1].active = true;
	abilities[1].usedTime = now;
	Unbreakable(true);
}

void R_Captain::Unbreakable(bool active) {
	if (active) {
		stats.armor = 1000;
	}
	else {
		stats.armor = baseStats.armor;
		abilities[1].active = false;
	}
}