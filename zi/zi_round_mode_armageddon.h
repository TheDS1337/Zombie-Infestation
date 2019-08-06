#ifndef _INCLUDE_ZI_ROUND_MODE_ARMAGEDDON_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_ARMAGEDDON_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class ArmageddonMode final: public ZIRoundMode
{
private:
	float m_SurvivorsToNemesisRatio;

public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;
	const char *GetSound() override;

	bool OnPreSelection();
	void OnPostSelection() override;
};

extern ConVar g_ArmageddonChance;
extern ConVar g_ArmageddonSurvivorsToNemesisRatio;

extern ArmageddonMode g_ArmageddonMode;

#endif