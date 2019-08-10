#ifndef _INCLUDE_ZI_ITEM_INFECTION_BOMB_PROPER_H_
#define _INCLUDE_ZI_ITEM_INFECTION_BOMB_PROPER_H_
#pragma once

#include "zi_items.h"

#define INFECTION_BOMB_NAME "Infection Bomb"
#define INFECTION_BOMB_VIP false
#define INFECTION_BOMB_COST 1

#define INFECTION_BOMB_LIMIT 5

class InfectionBombItem final: public ZIItem
{
private:
	int m_InfectionBombsCounter;

public:
	const char *GetName() override;
	bool IsVIP() override;
	int GetCost() override;

	ItemReturn OnPreSelection(ZIPlayer *player) override;
	void OnPostSelection(ZIPlayer *player) override;

	bool HasInfectionBomb(ZIPlayer *player);
	void OnPostRoundStart();
};

extern InfectionBombItem g_InfectionBombItem;

#endif