#include <map>
#include "zi_nades.h"
#include "zi_players.h"
#include "zi_zombies.h"
#include "zi_boss_survivor.h"
#include "zi_boss_sniper.h"
#include "zi_boss_nemesis.h"
#include "zi_boss_assassin.h"

#define FROST_EXPLOSION_RADIUS 240.0f
#define FROST_FREEZE_DURATION RandomFloat(3.0f, 6.0f)

#define INFECTION_EXPLOSION_RADIUS RandomFloat(150.0f, 200.0f)

static void UpdateHEGrenadeVelocity(void *data)
{
	BaseGrenade *nadeEnt = (BaseGrenade *) data;

	if( nadeEnt )
	{
		Vector newVelocity = nadeEnt->GetInitialVelocity() * 1.5f;

		nadeEnt->SetAbsVelocity(newVelocity);
//		SetGrenadeInitialVelocity(nadeEnt, initialVelocity);
	}
}

static void UpdateTAGrenadeVelocity(void *data)
{
	BaseGrenade *nadeEnt = (BaseGrenade *) data;

	if( nadeEnt )
	{
		Vector newVelocity = nadeEnt->GetInitialVelocity() * 0.7f;

		nadeEnt->SetAbsVelocity(newVelocity);
//		SetGrenadeInitialVelocity(nadeEnt, initialVelocity);

//		SetEntityCollisionGroup(nadeEnt, COLLISION_GROUP_PROJECTILE);		
	}
}

namespace ZINades
{
	void SetColor(BaseGrenade *nadeEnt, Color color)
	{
		// Set rendering
		nadeEnt->SetRenderFx(kRenderFxGlowShell);
		nadeEnt->SetRenderMode(kRenderGlow);
		nadeEnt->SetRenderColor(color);

		// Trail
		CellRecipientFilter filter;
		TE_BeamFollow(filter, 0.0f, ZIResources::m_BeamModelIndex, ZIResources::m_HaloModelIndex, 0, 100, 5.0f, 5.0f, 5.0f, 10, 1.0f, 5, color, 0, nadeEnt);
	}

	void OnPostHEGrenadeCreation(BaseGrenade *nadeEnt)
	{
		g_pSM->AddFrameAction(&UpdateHEGrenadeVelocity, nadeEnt);
	}

	void OnPostHEGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = new NadeData(NADE_TARGETNAME_EXPLOSIVE, owner->m_IsInfected);

		if( !data->infected )
		{
			data->color = Color(200, 0, 0, 200);
			SetColor(nadeEnt, data->color);

			// Just for the fun of testing
			nadeEnt->SetDamage(1000.0f);
			nadeEnt->SetDamageRadius(nadeEnt->GetDamageRadius() * 2.0f);
		}

		SET_NADE_DATA_ADDRESS(nadeEnt, data);
	}

	HookReturn OnPostHEGrenadeStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( !data->infected )
		{
			// Impact if it's a real player, bounce if it's not (Idea: maybe can make make impact possible when player toss nade on floor, but I dunno how to detect it yet.. maybe Z component of velocity?)
			nadeEnt->Detonate();
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	void OnPostFlashbangCreation(BaseGrenade *nadeEnt)
	{
	}

	void OnPostFlashbangSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = new NadeData(NADE_TARGETNAME_FROST, owner->m_IsInfected);

		if( !data->infected )
		{
			data->color = Color(0, 100, 200, 200);
			SetColor(nadeEnt, data->color);
		}

		SET_NADE_DATA_ADDRESS(nadeEnt, data);
	}

	HookReturn OnPreFlashbangThink(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( !data->infected )
		{
			HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	HookReturn OnPostFlashbangStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( data->infected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( VectorLength(nadeEnt->GetVelocity()) < 100.0f )
		{
			Vector origin = nadeEnt->GetOrigin();

			// Rings
			CellRecipientFilter filter;
			g_pExtension->m_pEngineSound->EmitSound(filter, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) nadeEnt)), CHAN_WEAPON, "ZombieInfestation/grenade_frost_explode.mp3", -1, "ZombieInfestation/grenade_frost_explode.mp3", VOL_NORM, ATTN_NORM, 0);

			TE_BeamRingPoint(filter, 0.0f, ZIResources::m_BeamModelIndex, ZIResources::m_HaloModelIndex, 0, 0, 0.5f, 5.0f, 5.0f, 0, 50.0f, 50, Color(0, 100, 200, 200), 0, origin, 0.0f, 385.0f);
			TE_BeamRingPoint(filter, 0.0f, ZIResources::m_BeamModelIndex, ZIResources::m_HaloModelIndex, 0, 0, 0.5f, 5.0f, 5.0f, 0, 40.0f, 50, Color(0, 100, 200, 200), 0, origin, 0.0f, 470.0f);
			TE_BeamRingPoint(filter, 0.0f, ZIResources::m_BeamModelIndex, ZIResources::m_HaloModelIndex, 0, 0, 0.5f, 5.0f, 5.0f, 0, 20.0f, 50, Color(0, 100, 200, 200), 0, origin, 0.0f, 550.0f);

			TE_Sparks(filter, 0.0f, origin, 5000, 1000, Vector(0.0f, 0.0f, 0.0f));

			ZIPlayer *nearby = nullptr;
			BasePlayer *nearbyEnt = nullptr;

			// FindEntityInSphere is not implemented!! (need to find signature, use entitylist.cpp for help)
			while( (nearbyEnt = (BasePlayer *) BaseEntity::FindEntityInSphere(nearbyEnt, "player", origin, FROST_EXPLOSION_RADIUS)) )
			{
				nearby = ZIPlayer::Find(nearbyEnt);

				if( !nearby || !nearby->m_IsAlive || !nearby->m_IsInfected || nearby->m_IsEnraged || GET_NEMESIS(nearby) || GET_ASSASSIN(nearby) )
				{
					continue;
				}

				nearby->Freeze(FROST_FREEZE_DURATION);
			}

			// Remove the entity
			nadeEnt->AcceptInput("Kill");
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	void OnPostSmokeGrenadeCreation(BaseGrenade *nadeEnt)
	{
	}

	void OnPostSmokeGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = new NadeData(NADE_TARGETNAME_TOXIC, owner->m_IsInfected);

		if( !data->infected )
		{
			data->color = Color(0, 200, 50, 200);
			SetColor(nadeEnt, data->color);
		}

		SET_NADE_DATA_ADDRESS(nadeEnt, data);
	}

	HookReturn OnPreSmokeGrenadeThink(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( !data->infected )
		{
			HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	HookReturn OnPreSmokeGrenadeDetonate(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( !data->infected )
		{
			HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	void OnPostMolotovCreation(BaseGrenade *nadeEnt)
	{
	}

	void OnPostMolotovSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = new NadeData(NADE_TARGETNAME_FLAME, owner->m_IsInfected);

		if( !data->infected )
		{
			data->color = Color(200, 100, 0, 200);
			SetColor(nadeEnt, data->color);

			nadeEnt->Ignite(3.0f);
		}

		SET_NADE_DATA_ADDRESS(nadeEnt, data);
	}

	void OnPostDecoyCreation(BaseGrenade *nadeEnt)
	{
	}

	void OnPostDecoySpawn(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		float r = RandomInt(100, 255);
		float g = RandomInt(100, 255);
		float b = RandomInt(100, 255);

		NadeData *data = new NadeData(NADE_TARGETNAME_FLARE, owner->m_IsInfected);

		if( !data->infected )
		{
			data->color = Color(r, g, g, 200);
			SetColor(nadeEnt, data->color);
		}

		SET_NADE_DATA_ADDRESS(nadeEnt, data);
	}

	HookReturn OnPreDecoyThink(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( !data->infected )
		{
			HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	HookReturn OnPostDecoyStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( data->infected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( VectorLength(nadeEnt->GetVelocity()) < 50.0f )
		{
			CellRecipientFilter filter;
			g_pExtension->m_pEngineSound->EmitSound(filter, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) nadeEnt)), CHAN_WEAPON, "ZombieInfestation/grenade_flare_on.mp3", -1, "ZombieInfestation/grenade_flare_on.mp3", VOL_NORM, ATTN_NORM, 0);
			/////
			static char sTime[128];

			BaseEntity *lightEnt = BaseEntity::CreateEntity("light_dynamic");

			// If entity isn't valid, then skip
			if( lightEnt )
			{
				ke::SafeSprintf(sTime, sizeof(sTime), "%d %d %d %d", data->color.r(), data->color.g(), data->color.b(), data->color.a());

				// Dispatch main values of the entity
				lightEnt->SetKeyValue("origin", nadeEnt->GetOrigin());
				lightEnt->SetKeyValue("inner_cone", "0");
				lightEnt->SetKeyValue("cone", "80");
				lightEnt->SetKeyValue("brightness", "1");
				lightEnt->SetKeyValue("pitch", "90");
				lightEnt->SetKeyValue("style", "5");
				lightEnt->SetKeyValue("_light", sTime);
				lightEnt->SetKeyValue("distance", 1000.0f);
				lightEnt->SetKeyValue("spotlight_radius", 300.0f);

				// Spawn the entity into the world
				lightEnt->Spawn();

				// Activate the entity
				lightEnt->AcceptInput("TurnOn");

				// Sets parent to the entity
				BaseEntity::SetInputVariant("!activator");
				lightEnt->AcceptInput("SetParent", nadeEnt, lightEnt);
				//			SetEntPropEnt(lightIndex, Prop_Data, "m_pParent", grenadeIndex);
			}

			// Initialize time char
			ke::SafeSprintf(sTime, sizeof(sTime), "OnUser1 !self:Kill::%f:1", 15.0f);

			// Sets modified flags on the entity
			BaseEntity::SetInputVariant(sTime);
			nadeEnt->AcceptInput("AddOutput");
			nadeEnt->AcceptInput("FireUser1");

			BaseEntity::SetInputVariant(sTime);
			lightEnt->AcceptInput("AddOutput");
			lightEnt->AcceptInput("FireUser1");

			ke::SafeSprintf(sTime, sizeof(sTime), "OnUser2 !self:TurnOff::%f:1", 14.0f);

			BaseEntity::SetInputVariant(sTime);
			lightEnt->AcceptInput("AddOutput");
			lightEnt->AcceptInput("FireUser2");
			/////	
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	HookReturn OnPreDecoyDetonate(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( !data->infected )
		{
			HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	void OnPostTAGrenadeCreation(BaseGrenade *nadeEnt)
	{
		nadeEnt->SetCollisionGroup(COLLISION_GROUP_PROJECTILE);
		g_pSM->AddFrameAction(&UpdateTAGrenadeVelocity, nadeEnt);
	}

	void OnPostTAGrenadeSpawn(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = new NadeData(NADE_TARGETNAME_INFECTION, owner->m_IsInfected);

		if( data->infected )
		{
			data->color = Color(0, 200, 0, 200);
			SetColor(nadeEnt, data->color);
		}

		SET_NADE_DATA_ADDRESS(nadeEnt, data);
	}

	HookReturn OnPreTAGrenadeThink(BaseGrenade *nadeEnt, ZIPlayer *owner)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( data->infected )
		{
			HOOK_RETURN_VOID(MRES_SUPERCEDE, false);
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	HookReturn OnPostTAGrenadeStartTouch(BaseGrenade *nadeEnt, ZIPlayer *owner, BaseEntity *otherEnt)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( data->infected != owner->m_IsInfected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( !data->infected )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		if( VectorLength(nadeEnt->GetVelocity()) < 50.0f )
		{
			Vector origin = nadeEnt->GetOrigin();

			// Rings
			CellRecipientFilter filter;
			g_pExtension->m_pEngineSound->EmitSound(filter, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) nadeEnt)), CHAN_WEAPON, "ZombieInfestation/grenade_infection_explode.mp3", -1, "ZombieInfestation/grenade_infection_explode.mp3", VOL_NORM, ATTN_NORM, 0);

			//		Color color(0, 200, 0, 200);

			//		TE_BeamRingPoint(filter, 0.0f, m_BeamModelIndex, -1, 0, 100, 5.0f, 50.0f, 50.0f, 5, 10.0f, 50, color, 0, origin, 0.0f, 385.0f);
			//		TE_BeamRingPoint(filter, 0.0f, m_BeamModelIndex, -1, 0, 100, 5.0f, 50.0f, 50.0f, 5, 10.0f, 50, color, 0, origin, 0.0f, 470.0f);
			//		TE_BeamRingPoint(filter, 0.0f, m_BeamModelIndex, -1, 0, 100, 5.0f, 50.0f, 50.0f, 5, 10.0f, 50, color, 0, origin, 0.0f, 550.0f);

			int infectedCount = 0;

			ZIPlayer *nearby = nullptr;
			BasePlayer *nearbyEnt = nullptr;

			// FindEntityInSphere is not implemented!! (need to find signature, use entitylist.cpp for help)
			while( (nearbyEnt = (BasePlayer *) BaseEntity::FindEntityInSphere(nearbyEnt, "player", origin, INFECTION_EXPLOSION_RADIUS)) )
			{
				nearby = ZIPlayer::Find(nearbyEnt);

				if( !nearby || !nearby->m_IsAlive || nearby->m_IsInfected || GET_SURVIVOR(nearby) || GET_SNIPER(nearby) )
				{
					continue;
				}

				if( nearby->m_IsLastHuman )
				{
					nearbyEnt->Kill();	// TODO: Change this to either event_killed or fake a very high damage to show the attacker deathnotice
				}
				else
				{
					nearby->Infect(owner);
				}

				// Make few of them screamy
				if( ++infectedCount == 1 || RandomInt(0, 2) == 0 )
				{
					CellRecipientFilter filter2;
					g_pExtension->m_pEngineSound->EmitSound(filter2, nearby->m_Index, CHAN_VOICE, "ZombieInfestation/grenade_infection_scream.mp3", -1, "ZombieInfestation/grenade_infection_scream.mp3", VOL_NORM, ATTN_NORM, 0);
				}
			}

			// Remove the entity
			nadeEnt->AcceptInput("Kill");
		}

		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	void OnPreNadeRemoval(BaseGrenade *nadeEnt)
	{
		NadeData *data = GET_NADE_DATA_ADDRESS(nadeEnt);

		if( !data )
		{
			return;
		}

		delete data; data = nullptr;

		SET_NADE_DATA_ADDRESS(nadeEnt, data);
	}
}