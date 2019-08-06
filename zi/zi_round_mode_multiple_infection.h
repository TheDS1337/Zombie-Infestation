#ifndef _INCLUDE_ZI_ROUND_MODE_MULTIPLE_INFECTION_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_MULTIPLE_INFECTION_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class MultipleInfectionMode final: public ZIRoundMode
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

extern ConVar g_MultipleInfectionChance;
extern ConVar g_MultipleInfectionRatio;

extern MultipleInfectionMode g_MultipleInfectionMode;

#endif