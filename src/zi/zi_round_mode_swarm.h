#ifndef _INCLUDE_ZI_ROUND_MODE_SWARM_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_SWARM_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class SwarmMode final: public ZIRoundMode
{
private:
	float m_InfectionRatio;

public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;
	const char *GetSound() override;

	bool OnPreSelection();
	void OnPostSelection() override;
};

extern ConVar g_SwarmChance;
extern ConVar g_SwarmInfectionRatio;

extern SwarmMode g_SwarmMode;

#endif