#ifndef _INCLUDE_ZI_ROUND_MODE_ASSASSINS_VS_SNIPERS_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODE_ASSASSINS_VS_SNIPERS_PROPER_H_
#pragma once

#include "zi_round_modes.h"

class AssassinsVSSnipersMode final: public ZIRoundMode
{
private:
	float m_SnipersToAssassinsRatio;

public:
	const char *GetName() override;
	int GetChance() override;
	bool IsInfectionAllowed() override;
	const char *GetSound() override;

	bool OnPreSelection();
	void OnPostSelection() override;
};

extern ConVar g_AvSChance;
extern ConVar g_AvSSnipersToAssassinsRatio;

extern AssassinsVSSnipersMode g_AvSMode;

#endif