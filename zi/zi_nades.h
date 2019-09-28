#ifndef _INCLUDE_ZI_NADES_PROPER_H_
#define _INCLUDE_ZI_NADES_PROPER_H_
#pragma once

#include "zi_core.h"

#define NADE_CONCUSSION_TARGETNAME	"zi_nade_concussion"

#define NADE_EXPLOSIVE_TARGETNAME	"zi_nade_explosive"
#define NADE_EXPLOSIVE_COLOR Color(200, 0, 0, 200)

#define NADE_FROST_TARGETNAME		"zi_nade_frost"
#define NADE_FROST_COLOR Color(0, 100, 200, 200)

#define NADE_TOXIC_TARGETNAME		"zi_nade_toxic"
#define NADE_TOXIC_COLOR Color(0, 200, 50, 200)

#define NADE_FLAME_TARGETNAME		"zi_nade_napalm"
#define NADE_FLAME_COLOR Color(200, 100, 0, 200)

#define NADE_FLARE_TARGETNAME		"zi_nade_flare"
#define NADE_FLARE_DURATION RandomFloat(15.0f, 25.0f)
#define NADE_FLARE_COLOR Color(RandomInt(0, 255), RandomInt(0, 255), RandomInt(0, 255), 200)
#define NADE_FLARE_DISTANCE 600.0f
#define NADE_FLARE_RADIUS 150.0f

#define NADE_INFECTION_TARGETNAME	"zi_nade_infection"
#define NADE_INFECTION_COLOR Color(0, 200, 0, 200)

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