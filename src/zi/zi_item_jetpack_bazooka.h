#ifndef _INCLUDE_ZI_ITEM_JETPACK_BAZOOKA_PROPER_H_
#define _INCLUDE_ZI_ITEM_JETPACK_BAZOOKA_PROPER_H_
#pragma once

#include "zi_items.h"

#define JETPACK_BAZOOKA_NAME "Jetpack/Bazooka"
#define JETPACK_BAZOOKA_VIP false
#define JETPACK_BAZOOKA_COST 1
#define JETPACK_BAZOOKA_TARGETNAME "zi_jetpack"
#define JETPACK_BAZOOKA_ROCKET_TARGETNAME "zi_jetpack_rocket"
#define JETPACK_BAZOOKA_EXPLOSION_RADIUS 300.0f
#define JETPACK_BAZOOKA_EXPLOSION_DAMAGE RandomFloat(300.0f, 600.0f)

// I'll stock the owner into the entity's hacked offset, because setting the owner manually will make the tripmine SOLID_NONE to them....
#define GET_ROCKET_OWNER(tripmineEnt) *(BasePlayer **) ((char *) tripmineEnt + g_pExtension->m_OnUser4)
#define SET_ROCKET_OWNER(tripmineEnt, address) *(BasePlayer **) ((char *) tripmineEnt + g_pExtension->m_OnUser4) = address
#define GET_ROCKET_DATA(tripmineEnt) *(RocketData **) ((char *) tripmineEnt + g_pExtension->m_OnUser3)
#define SET_ROCKET_DATA(tripmineEnt, address) *(RocketData **) ((char *) tripmineEnt + g_pExtension->m_OnUser3) = address

struct RocketData
{
	BaseEntity *entity;
	Vector dir;

	RocketData(BaseEntity *entity, Vector &dir)
	{
		this->entity = entity;
		this->dir = dir;
	}

	~RocketData()
	{		
		this->entity = nullptr;
	}
};

class JetpackBazookaItem final: public ZIItem
{
public:
	int m_BeamModelIndex;
private:
//	int m_BeamModelIndex;
	int m_FireModelIndex;
	int m_ExplosionModelIndex;

//	void UpdateRocketVelocity(void *data);

public:
	const char *GetName() override;
	bool IsVIP() override;
	int GetCost() override;

	void OnPostSelection(ZIPlayer *player) override;

	void Precache();
	void GetOffsets();

	HookReturn OnPostClientRunCommand(ZIPlayer *player, CUserCmd *userCmd, IMoveHelper *moveHelper);
	void OnPostClientInfection(ZIPlayer *player);
	void OnPostClientDeath(ZIPlayer *player);	

	void OnPostRocketSpawn();
	void OnPostRocketStartTouch(BaseEntity *other);
};

extern JetpackBazookaItem g_JetpackBazookaItem;

#endif