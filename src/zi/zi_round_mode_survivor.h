#ifndef _INCLUDE_ZI_ROUND_MODE_SURVIVOR_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_SURVIVOR_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class SurvivorMode final: public ZIRoundMode
{
public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;
	const char *GetSound() override;

//	bool OnPreSelection();
	void OnPostSelection() override;
};

extern ConVar g_SurvivorChance;

extern SurvivorMode g_SurvivorMode;

#endif