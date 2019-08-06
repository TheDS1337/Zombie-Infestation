#include "zi_item_jetpack_bazooka.h"
#include "zi_resources.h"

JetpackBazookaItem g_JetpackBazookaItem;

SH_DECL_MANUALHOOK0_void(RocketSpawn, 0, 0, 0)
SH_DECL_MANUALHOOK1_void(RocketStartTouch, 0, 0, 0, BaseEntity *)

const char *JetpackBazookaItem::GetName()
{
	return JETPACK_BAZOOKA_NAME;
}

bool JetpackBazookaItem::IsVIP()
{
	return JETPACK_BAZOOKA_VIP;
}

int JetpackBazookaItem::GetCost()
{
	return JETPACK_BAZOOKA_COST;
}

void JetpackBazookaItem::OnPostSelection(ZIPlayer *player)
{
	player->m_HasJetpack = true;
	player->m_JetpackFuel = 500;
	player->m_NextRocketTime = g_pExtension->m_pGlobals->curtime;
}

void JetpackBazookaItem::Precache()
{
	PRECACHE_SOUND("ZombieInfestation/jetpack_fly.mp3");
	PRECACHE_SOUND("ZombieInfestation/jetpack_blow.mp3");
	PRECACHE_SOUND("ZombieInfestation/jetpack_rocket.mp3");

	PRECACHE_MODEL("models/weapons/w_missile_launch.mdl");

	AddFileToDownloadsTable("models/weapons/w_missile_launch.mdl");
	AddFileToDownloadsTable("materials/models/weapons/w_missile/missile side.vmt");

	m_BeamModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/laserbeam.vmt", true);
	m_FireModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/xfireball3.vmt", true);
	m_ExplosionModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/zerogxplode.vmt", true);
}

void JetpackBazookaItem::GetOffsets()
{
	int offset = 0;

	SH_MANUAL_HOOK_GET_OFFSET(RocketSpawn, "Spawn");
	SH_MANUAL_HOOK_GET_OFFSET(RocketStartTouch, "StartTouch");
}

void UpdateRocketVelocity(void *data)
{	
	RocketData *rocketData = (RocketData *) data;
	BaseEntity *rocketEnt = rocketData->entity;

	if( rocketEnt )
	{
		Vector newVelocity = rocketData->dir * 2500.0f;

		rocketEnt->SetAbsVelocity(newVelocity);
//		SetGrenadeInitialVelocity(rocketEnt, initialVelocity);		
	}

	delete rocketData;
}

HookReturn JetpackBazookaItem::OnPostClientRunCommand(ZIPlayer *player, CUserCmd *userCmd, IMoveHelper *moveHelper)
{
	if( !player->m_IsAlive || !player->m_HasJetpack )
	{
		HOOK_RETURN_VOID(MRES_IGNORED, false);
	}

	BasePlayer *playerEnt = player->m_pEntity;

	int buttons = userCmd->buttons;
	float currentTime = g_pExtension->m_pGlobals->curtime;

	if( buttons & IN_JUMP && buttons & IN_DUCK  )
	{ 
		if( playerEnt->GetFlags() & FL_ONGROUND || player->m_JetpackFuel < 1 )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}	

		Vector boostVelocity = playerEnt->GetVelocity();

		float zVelocity = boostVelocity.z;

		if( zVelocity < 300.0f )
		{
			zVelocity += 35.0f;
		}
		
		QAngle angles = playerEnt->GetAngles();
//		QAngle angles = playerEnt->GetEyeAngles();
		angles.z = 0.0f;

		AngleVectors(angles, &boostVelocity);

		boostVelocity.x *= RandomFloat(270.0f, 320.0f);
		boostVelocity.y *= RandomFloat(270.0f, 320.0f);
		boostVelocity.z = zVelocity;

		playerEnt->Teleport(nullptr, nullptr, &boostVelocity);

		CellRecipientFilter filter;	

		if( RandomInt(0, 3) == 0 )
		{
			TE_Sprite(filter, 0.0f, playerEnt->GetOrigin(), m_FireModelIndex, 0.8f, 200);
		}

		CellRecipientFilter filter2;

		if( player->m_JetpackFuel > 150 )
		{
			g_pExtension->m_pEngineSound->EmitSound(filter2, player->m_Index, CHAN_ITEM, "ZombieInfestation/jetpack_fly.mp3", -1, "ZombieInfestation/jetpack_fly.mp3", VOL_NORM, ATTN_NORM, 0);
		}
		else
		{
			g_pExtension->m_pEngineSound->EmitSound(filter2, player->m_Index, CHAN_ITEM, "ZombieInfestation/jetpack_blow.mp3", -1, "ZombieInfestation/jetpack_blow.mp3", VOL_NORM, ATTN_NORM, 0);
		}

		player->m_JetpackFuel -= 2;
	}
	else if( player->m_JetpackFuel < 500 )
	{
		player->m_JetpackFuel++;
	}

	if( buttons & IN_ATTACK2 )
	{
		if( player->m_NextRocketTime > currentTime )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		IPlayerInfo *info = player->m_pGamePlayer->GetPlayerInfo();

		if( !info )
		{
			HOOK_RETURN_VOID(MRES_IGNORED, false);
		}

		// TODO: create the rocket	
		BaseEntity *rocketEnt = BaseEntity::CreateEntity("hegrenade_projectile");

		if( rocketEnt )
		{
			QAngle angles = playerEnt->GetEyeAngles();
			Vector pos = playerEnt->GetOrigin();

			Vector dir;
			AngleVectors(angles, &dir);

			pos.x += 64.0f * dir.x;
			pos.y += 64.0f * dir.y;	
			pos.z += 35.0f;

			rocketEnt->SetKeyValue("targetname", JETPACK_BAZOOKA_ROCKET_TARGETNAME);
			rocketEnt->SetKeyValue("model", "models/weapons/w_missile_launch.mdl");
			rocketEnt->SetKeyValue("origin", pos);
			rocketEnt->SetKeyValue("angles", angles);

			// Schedule the velocity update for next frame, otherwise it won't work
			g_pSM->AddFrameAction(&UpdateRocketVelocity, new RocketData(rocketEnt, dir));

			SH_MANUAL_HOOK_CREATE(Rocket_PostSpawn, rocketEnt, SH_ADD_MANUALVPHOOK(RocketSpawn, rocketEnt, SH_MEMBER(&g_JetpackBazookaItem, &JetpackBazookaItem::OnPostRocketSpawn), true));
			SH_MANUAL_HOOK_CREATE(Rocket_PostStartTouch, rocketEnt, SH_ADD_MANUALVPHOOK(RocketStartTouch, rocketEnt, SH_MEMBER(&g_JetpackBazookaItem, &JetpackBazookaItem::OnPostRocketStartTouch), true));

			rocketEnt->Spawn();

			rocketEnt->SetSize(Vector(-30.0f, -2.0f, -2.0f), Vector(30.0f, 2.0f, 2.0f));
			rocketEnt->SetCollisionGroup(COLLISION_GROUP_DEBRIS_TRIGGER);
			rocketEnt->SetMoveType(MOVETYPE_FLY);
			
			SET_ROCKET_OWNER(rocketEnt, player->m_pEntity);		
		}

		player->m_NextRocketTime = currentTime + 15.0f;
	}	

	HOOK_RETURN_VOID(MRES_IGNORED, false);
}

void JetpackBazookaItem::OnPostClientInfection(ZIPlayer *player)
{
	if( !player->m_HasJetpack )
	{
		return;
	}

	// TODO: Drop jet

	player->m_HasJetpack = false;	
}

void JetpackBazookaItem::OnPostClientDeath(ZIPlayer *player)
{
	if( !player->m_HasJetpack )
	{
		return;
	}

	// TODO: Drop jet

	player->m_HasJetpack = false;	
}

void JetpackBazookaItem::OnPostRocketSpawn()
{
	BaseEntity *rocketEnt = (BaseEntity *) META_IFACEPTR(CBaseEntity);	

	CellRecipientFilter filter;
	TE_BeamFollow(filter, 0.0f, g_JetpackBazookaItem.m_BeamModelIndex, 0, 0, 100, 2.5f, 8.0f, 8.0f, 10, 1.0f, 5, Color(200, 200, 200, RandomInt(215, 255)), 0, rocketEnt);

	CellRecipientFilter filter2;
	g_pExtension->m_pEngineSound->EmitSound(filter2, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) rocketEnt)), CHAN_WEAPON, "ZombieInfestation/jetpack_rocket.mp3", -1, "ZombieInfestation/jetpack_rocket.mp3", VOL_NORM, ATTN_NORM, 0);
}

void JetpackBazookaItem::OnPostRocketStartTouch(BaseEntity *other)
{
	BaseEntity *rocketEnt = (BaseEntity *) META_IFACEPTR(CBaseEntity);
	ZIPlayer *owner = ZIPlayer::Find(GET_ROCKET_OWNER(rocketEnt));

	if( !owner )
	{ 
		RETURN_META(MRES_IGNORED);
	}

	Vector center = rocketEnt->GetOrigin();

	CellRecipientFilter filter;
	TE_Explosion(filter, 0.0f, center, m_ExplosionModelIndex, 8.5f, 15, 0, Vector(0.0f, 0.0f, 1.0f), 'C', JETPACK_BAZOOKA_EXPLOSION_RADIUS, 1000);	

	const char *sound = ZIResources::GetRandomExplosionSound();

	CellRecipientFilter filter2;
	g_pExtension->m_pEngineSound->EmitSound(filter2, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) rocketEnt)), CHAN_STATIC, sound, -1, sound, VOL_NORM, ATTN_NORM, 0);

	ZIPlayer *nearby = nullptr;
	BasePlayer *nearbyEnt = nullptr;
	int *nearbyIndex = nullptr;

	Vector origin;
	float distance = 0.0f, distanceRatio = 0.0f;

	// FindEntityInSphere is not implemented!! (need to find signature, use entitylist.cpp for help)
	while( (nearbyEnt = (BasePlayer *) BaseEntity::FindEntityInSphere(nearbyEnt, "player", center, JETPACK_BAZOOKA_EXPLOSION_RADIUS)) )
	{
		nearby = ZIPlayer::Find(nearbyEnt);

		if( !nearby || !nearby->m_IsAlive || !nearby->m_IsInfected )
		{
			continue;
		}

		origin = nearbyEnt->GetOrigin();

		distance = VectorLength(origin - center);
		distanceRatio = 1 - distance / JETPACK_BAZOOKA_EXPLOSION_RADIUS;

		nearbyIndex = &nearby->m_Index;
		nearbyEnt->TakeDamage(CTakeDamageInfo2((CBaseEntity *) rocketEnt, (CBaseEntity *) GET_ROCKET_OWNER(rocketEnt), JETPACK_BAZOOKA_EXPLOSION_DAMAGE * distanceRatio, DMG_BLAST, (CBaseEntity *) rocketEnt, Vector(0.0f, 0.0f, 0.0f), center));

		UM_ScreenFade(nearbyIndex, 1, RandomInt(600, 1000) * distanceRatio, 0, FFADE_IN, Color(255, 0, 0, 255));
		UM_ScreenShake(nearbyIndex, 1, SHAKE_START, RandomFloat(10.0f, 15.0f) * distanceRatio, RandomFloat(1.0f, 5.0f), RandomFloat(0.8f, 1.3f));

		// Closer, the danger
		if( distanceRatio > RandomFloat(0.5f, 0.7f) )
		{
			nearby->PlaySound("ZombieInfestation/flatline.mp3");
		}
	}

	// Remove the entity
	rocketEnt->AcceptInput("Kill");
}