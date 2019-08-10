#include "zi_item_rage.h"
#include "zi_zombies.h"
#include "zi_boss_nemesis.h"
#include "zi_boss_assassin.h"
#include "zi_timers.h"

RageItem g_RageItem;

const char *RageItem::GetName()
{
	return RAGE_NAME;
}

bool RageItem::IsVIP()
{
	return RAGE_VIP;
}

int RageItem::GetCost()
{
	return RAGE_COST;
}

ItemReturn RageItem::OnPreSelection(ZIPlayer *player)
{
	if( !player->m_IsInfected || GET_NEMESIS(player) || GET_ASSASSIN(player)  )
	{
		return ItemReturn_DontShow;
	}
	else if( ZICore::m_IsRoundEnd )
	{
		return ItemReturn_NotAvailable;
	}

	return ItemReturn_Show;
}

void RageItem::OnPostSelection(ZIPlayer *player)
{	
	BasePlayer *playerEnt = player->m_pEntity;

	// If he was already frozen, unfreeze him
	if( player->m_IsFrozen )
	{
		player->Unfreeze();
	}

	// Was on fire? extinguish him
	if( playerEnt->GetFlags() & FL_ONFIRE )
	{
		playerEnt->Extinguish();
	}

	playerEnt->SetSpeed(RAGE_PLAYER_SPEED);

	// Give him some kind of a raging glow
	player->m_pGlowEntity = CreateEntityGlow(player->m_pEntity, 2, Color(200, 0, 50, 100), "primary");

	// TODO: Add sound
	CellRecipientFilter filter;	

	player->m_IsEnraged = true;
	player->m_pZombieCooldownTimer = timersys->CreateTimer(&m_ZombieCooldownTimerCallback, RandomFloat(4.0f, 6.0f), player, TIMER_FLAG_NO_MAPCHANGE);
}

void RageItem::OnPostClientInfection(ZIPlayer *player)
{
	RELEASE_TIMER(player->m_pZombieCooldownTimer);
	ZombieCooldown(player);
}

void RageItem::OnPostClientDisinfection(ZIPlayer *player)
{
	RELEASE_TIMER(player->m_pZombieCooldownTimer);
	ZombieCooldown(player);
}

HookReturn RageItem::OnPreClientTraceAttack(ZIPlayer *player, CTakeDamageInfo2 &info)
{
	ZIPlayer *attacker = ZIPlayer::Find(gamehelpers->ReferenceToIndex(info.GetAttacker()), false);

	if( !attacker )
	{
		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	if( !player->m_IsEnraged )
	{
		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
}

void RageItem::OnPostClientDeath(ZIPlayer *player)
{
	RELEASE_TIMER(player->m_pZombieCooldownTimer);
	ZombieCooldown(player);
}

void RageItem::ZombieCooldown(ZIPlayer *player)
{
	BaseProp *glowEnt = player->m_pGlowEntity;

	// Remove the glow
	if( player->m_pGlowEntity )
	{
		glowEnt->AcceptInput("Kill"); player->m_pGlowEntity = nullptr;
	}

	// Reset the speed to the default class's speed
	if( player->m_pHumanLike )
	{
		player->m_pEntity->SetSpeed(player->m_pHumanLike->GetSpeed());
	}	

	player->m_IsEnraged = false;
}

ResultType RageItem::ZombieCooldownTimerCallback::OnTimer(ITimer *timer, void *data)
{
	ZIPlayer *player = (ZIPlayer *) data;

	if( !player )
	{
		return Pl_Continue;
	}

	g_RageItem.ZombieCooldown(player);

	player->m_pZombieCooldownTimer = nullptr;
	return Pl_Stop;
}

void RageItem::ZombieCooldownTimerCallback::OnTimerEnd(ITimer *timer, void *data)
{
}