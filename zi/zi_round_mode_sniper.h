#ifndef _INCLUDE_ZI_ROUND_MODE_SNIPER_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_SNIPER_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class SniperMode final: public ZIRoundMode
{
public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;
	const char *GetSound() override;

//	bool OnPreSelection();
	void OnPostSelection() override;
};

extern ConVar g_SniperChance;

extern SniperMode g_SniperMode;

#endif