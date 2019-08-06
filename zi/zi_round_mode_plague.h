#ifndef _INCLUDE_ZI_ROUND_MODE_PLAGUE_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_PLAGUE_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class PlagueMode final: public ZIRoundMode
{
private:
	float m_BossRatio;
	float m_SurvivorsToNemesisRatio;
	float m_InfectionRatio;

public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;
	const char *GetSound() override;

	bool OnPreSelection();
	void OnPostSelection() override;
};

extern ConVar g_PlagueChance;
extern ConVar g_PlagueBossRatio;
extern ConVar g_PlagueSurvivorsToNemesisRatio;
extern ConVar g_PlagueInfectionRatio;

extern PlagueMode g_PlagueMode;

#endif