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

class ZINades
{
public:
	static int m_BeamModelIndex;
	static int m_HaloModelIndex;
	static int m_GlassModelIndex[6];	

	static void SetColor(BaseGrenade *nadeEnt, Color color);	

	static void Precache();	

	static void OnPostHEGrenadeCreation(BaseGrenade *nadeEnt);
	static void OnPostHEGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPostHEGrenadeStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt);
	
	static void OnPostFlashbangCreation(BaseGrenade *nadeEnt);
	static void OnPostFlashbangSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPreFlashbangThink(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPostFlashbangStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt);
	
	static void OnPostSmokeGrenadeCreation(BaseGrenade *nadeEnt);
	static void OnPostSmokeGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPreSmokeGrenadeThink(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPreSmokeGrenadeDetonate(BaseGrenade *nadeEnt, ZIPlayer *owner);

	static void OnPostMolotovCreation(BaseGrenade *nadeEnt);
	static void OnPostMolotovSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);

	static void OnPostDecoyCreation(BaseGrenade *nadeEnt);
	static void OnPostDecoySpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPreDecoyThink(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPostDecoyStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt);
	static HookReturn OnPreDecoyDetonate(BaseGrenade *nadeEnt, ZIPlayer *owner);

	static void OnPostTAGrenadeCreation(BaseGrenade *nadeEnt);
	static void OnPostTAGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPreTAGrenadeThink(BaseGrenade *nadeEnt, ZIPlayer *owner);
	static HookReturn OnPostTAGrenadeStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt);

	static void OnPreNadeRemoval(BaseGrenade *nadeEnt);	
};

extern ZINades g_Nades;


#endif 