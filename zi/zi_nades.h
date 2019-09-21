#ifndef _INCLUDE_ZI_NADES_PROPER_H_
#define _INCLUDE_ZI_NADES_PROPER_H_
#pragma once

#include "zi_core.h"

#define NADE_TARGETNAME_EXPLOSIVE	"zi_nade_explosive"
#define NADE_TARGETNAME_FROST		"zi_nade_frost"
#define NADE_TARGETNAME_FLAME		"zi_nade_napalm"
#define NADE_TARGETNAME_CONCUSSION	"zi_nade_concussion"
#define NADE_TARGETNAME_FLARE		"zi_nade_flare"
#define NADE_TARGETNAME_TOXIC		"zi_nade_toxic"
#define NADE_TARGETNAME_INFECTION	"zi_nade_infection"

// We'll be using 4 byes of this Vector, since it's not really used after the nade creation so we'll survive
#define GET_NADE_DATA_ADDRESS(nadeEnt) *(NadeData **) ((char *) nadeEnt + g_pExtension->m_OnUser4)
#define SET_NADE_DATA_ADDRESS(nadeEnt, address) *(NadeData **) ((char *) nadeEnt + g_pExtension->m_OnUser4) = address

struct NadeData
{
	const char *targetname;
	bool infected;
	Color color;

	NadeData(const char *targetname, bool infected)
	{
		this->targetname = targetname;
		this->infected = infected;		
	}

	~NadeData()
	{
		this->targetname = nullptr;
		this->infected = false;		
	}
};

namespace ZINades
{
	void SetColor(BaseGrenade *nadeEnt, Color color);	

	void OnPostHEGrenadeCreation(BaseGrenade *nadeEnt);
	void OnPostHEGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPostHEGrenadeStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt);
	
	void OnPostFlashbangCreation(BaseGrenade *nadeEnt);
	void OnPostFlashbangSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPreFlashbangThink(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPostFlashbangStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt);
	
	void OnPostSmokeGrenadeCreation(BaseGrenade *nadeEnt);
	void OnPostSmokeGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPreSmokeGrenadeThink(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPreSmokeGrenadeDetonate(BaseGrenade *nadeEnt, ZIPlayer *owner);

	void OnPostMolotovCreation(BaseGrenade *nadeEnt);
	void OnPostMolotovSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);

	void OnPostDecoyCreation(BaseGrenade *nadeEnt);
	void OnPostDecoySpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPreDecoyThink(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPostDecoyStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt);
	HookReturn OnPreDecoyDetonate(BaseGrenade *nadeEnt, ZIPlayer *owner);

	void OnPostTAGrenadeCreation(BaseGrenade *nadeEnt);
	void OnPostTAGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPreTAGrenadeThink(BaseGrenade *nadeEnt, ZIPlayer *owner);
	HookReturn OnPostTAGrenadeStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt);

	void OnPreNadeRemoval(BaseGrenade *nadeEnt);	
};

#endif 