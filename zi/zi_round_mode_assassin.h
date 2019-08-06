#ifndef _INCLUDE_ZI_ROUND_MODE_ASSASSIN_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_ASSASSIN_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class AssassinMode final: public ZIRoundMode
{
public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;
	const char *GetSound() override;

//	virtual bool OnPreSelection();
	virtual void OnPostSelection() override;
};

extern ConVar g_AssassinChance;

extern AssassinMode g_AssassinMode;

#endif