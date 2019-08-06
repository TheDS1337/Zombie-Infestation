#ifndef _INCLUDE_ZI_ROUND_MODE_SINGLE_INFECTION_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_SINGLE_INFECTION_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class SingleInfectionMode final: public ZIRoundMode
{
public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;

//	bool OnPreSelection();
	void OnPostSelection() override;
};

extern ConVar g_SingleInfectionChance;

extern SingleInfectionMode g_SingleInfectionMode;

#endif