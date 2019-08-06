#ifndef _INCLUDE_ZI_ROUND_MODE_NIGHTMARE_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_NIGHTMARE_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class NightmareMode final: public ZIRoundMode
{
private:
	float m_InfectionRatio;
	float m_SurvivorsToSnipersRatio;
	float m_NemesisToAssassinsRatio;

public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;
	const char *GetSound() override;

	bool OnPreSelection();
	void OnPostSelection() override;
};

extern ConVar g_NightmareChance;
extern ConVar g_NightmareInfectionRatio;
extern ConVar g_NightmareSurvivorsToSnipersRatio;
extern ConVar g_NightmareNemesisToAssassinsRatio;

extern NightmareMode g_NightmareMode;

#endif