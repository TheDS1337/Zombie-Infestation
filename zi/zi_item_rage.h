#ifndef _INCLUDE_ZI_ITEM_RAGE_PROPER_H_
#define _INCLUDE_ZI_ITEM_RAGE_PROPER_H_
#pragma once

#include "zi_items.h"

#define RAGE_NAME "Rage"
#define RAGE_VIP false
#define RAGE_COST 1

#define RAGE_PLAYER_SPEED 300.0f

class RageItem final: public ZIItem
{
private:
	class ZombieCooldownTimerCallback final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	ZombieCooldownTimerCallback m_ZombieCooldownTimerCallback;

public:
	const char *GetName() override;
	bool IsVIP() override;
	int GetCost() override;

	ItemReturn OnPreSelection(ZIPlayer *player) override;
	void OnPostSelection(ZIPlayer *player) override;

	void OnPostClientInfection(ZIPlayer *player);
	void OnPostClientDisinfection(ZIPlayer *player);
	HookReturn OnPreClientTraceAttack(ZIPlayer *player, CTakeDamageInfo2 &info);
	void OnPostClientDeath(ZIPlayer *player);

	void ZombieCooldown(ZIPlayer *player);
};

extern RageItem g_RageItem;

#endif