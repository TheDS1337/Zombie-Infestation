#include "zi_item_tripmine.h"
#include "zi_core.h"
#include "zi_commands.h"
#include "zi_timers.h"
#include "zi_resources.h"

static char CMD_PLANT_NAME[] = "mine_plant";
static char CMD_PLANT_DESCRIPTION[] = "Plants the tripmine";

static char CMD_TAKE_NAME[] = "mine_take";
static char CMD_TAKE_DESCRIPTION[] = "Takes the tripmine";

class CTraceFilterTripmines final: public CTraceFilter
{
public:
	CTraceFilterTripmines(ZIPlayer *player, bool take)
	{
		this->m_Player = player;
		this->m_IsTakingMine = take;	
	}

	bool ShouldHitEntity(IHandleEntity *serverEntity, int contentsMask) override
	{
		BaseEntity *entity = (BaseEntity *) serverEntity;

		if( m_IsTakingMine )
		{
			char targetname[32];
			entity->GetKeyValue("targetname", targetname, sizeof(targetname));

			if( strcmp(targetname, TRIPMINE_TARGETNAME) == 0 )
			{
				return GET_TRIPMINE_OWNER(entity) == m_Player->m_pEntity;
			}
		}
		
		// Hit any entity except the owner
		else if( m_Player->m_pEntity != entity )
		{
			return true;
		}

		return false;
	}

private:
	ZIPlayer *m_Player;
	bool m_IsTakingMine;
};

TripmineItem g_TripmineItem;

SH_DECL_MANUALHOOK1(TripmineTakeDamage, 0, 0, 0, int, CTakeDamageInfo2 &)

BaseEntity *TripmineItem::LookupForTripmine(ZIPlayer *player, bool take)
{
	Vector dir;
	AngleVectors(player->m_pEntity->GetEyeAngles(), &dir);

	Vector eyePos;
	g_pExtension->m_pServerGameClients->ClientEarPosition(player->m_pEdict, &eyePos);

	Ray_t ray;
	ray.Init(eyePos, eyePos + dir * TRIPMINE_MAX_DISTANCE);

	trace_t trace;
	CTraceFilterTripmines filter(player, take);

	g_pExtension->m_pEngineTrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

	if( trace.DidHit() )
	{
		return (BaseEntity *) trace.m_pEnt;
	}

	return nullptr;
}

const char *TripmineItem::GetName()
{
	return TRIPMINE_NAME;
}

bool TripmineItem::IsVIP()
{
	return TRIPMINE_VIP;
}

int TripmineItem::GetCost()
{
	return TRIPMINE_COST;
}

ItemReturn TripmineItem::OnPreSelection(ZIPlayer *player)
{
	if( player->m_IsInfected )
	{
		return ItemReturn_DontShow;
	}
	else if( ZICore::m_IsRoundEnd )
	{
		return ItemReturn_NotAvailable;
	}
	else if( player->m_HasTripmine )
	{
		UM_SayText(&player->m_Index, 1, 0, true, "You already have a \x05planted \x04Tripmine\x01.");
		return ItemReturn_NotAvailable;
	}

	return ItemReturn_Show;
}

void TripmineItem::OnPostSelection(ZIPlayer *player)
{	
	UM_SayText(&player->m_Index, 1, 0, true, "You just bought a \x04Tripmine\x01! use it to block \x03zombies \x01in narrow places.");

	player->m_HasTripmine = true;
}

void TripmineItem::OnLoad()
{
	m_pPlantTripmineCmd = new ConCommand(CMD_PLANT_NAME, this, CMD_PLANT_DESCRIPTION);
	m_pTakeTripmineCmd = new ConCommand(CMD_TAKE_NAME, this, CMD_TAKE_DESCRIPTION);

	g_pExtension->m_pConsoleVars->RegisterConCommand(m_pPlantTripmineCmd);
	g_pExtension->m_pConsoleVars->RegisterConCommand(m_pTakeTripmineCmd);
}

void TripmineItem::OnUnload()
{
	delete m_pPlantTripmineCmd;
	delete m_pTakeTripmineCmd;
}

void TripmineItem::Precache()
{
	PRECACHE_SOUND("ZombieInfestation/tripmine_deploy.mp3");
	PRECACHE_SOUND("ZombieInfestation/tripmine_charge.mp3");
	PRECACHE_SOUND("ZombieInfestation/tripmine_activate.mp3");

	PRECACHE_MODEL("models/weapons/eminem/laser_mine/w_laser_mine_dropped3.mdl");

	AddFileToDownloadsTable("models/weapons/eminem/laser_mine/w_laser_mine_dropped3.mdl");
	AddFileToDownloadsTable("models/weapons/eminem/laser_mine/w_laser_mine_dropped3.vvd");
	AddFileToDownloadsTable("models/weapons/eminem/laser_mine/w_laser_mine_dropped3.dx90.vtx");
	AddFileToDownloadsTable("models/weapons/eminem/laser_mine/w_laser_mine_dropped3.phy");
	AddFileToDownloadsTable("materials/models/weapons/eminem/laser_mine/combine_mine.vtf");
	AddFileToDownloadsTable("materials/models/weapons/eminem/laser_mine/combine_mine_normal.vtf");
	AddFileToDownloadsTable("materials/models/weapons/eminem/laser_mine/combine_mine.vmt");

	m_ExplosionModelIndex = g_pExtension->m_pEngineServer->PrecacheModel("materials/sprites/zerogxplode.vmt", true);
}

void TripmineItem::GetOffsets()
{
	int offset = 0;

	SH_MANUAL_HOOK_GET_OFFSET(TripmineTakeDamage, "OnTakeDamage");
}

void TripmineItem::OnPostEntityCreation(BaseEntity *entity, const char *classname)
{
	if( strcmp(classname, "prop_physics_override") != 0 )
	{
		return;
	}

	static char targetname[32];
	entity->GetKeyValue("targetname", targetname, sizeof(targetname));

	if( strcmp(targetname, TRIPMINE_TARGETNAME) == 0 )
	{
//		SH_MANUAL_HOOK_CREATE(Tripmine_PreTakeDamage, entity, SH_ADD_MANUALVPHOOK(TripmineTakeDamage, entity, SH_MEMBER(this, &TripmineItem::OnPreMineTakeDamage), false));
	}
}

bool TripmineItem::OnPreEntityDestruction(BaseEntity *entity, const char *classname)
{
	if( strcmp(classname, "prop_physics_override") != 0 )
	{
		return true;
	}

	static char targetname[32];
	entity->GetKeyValue("targetname", targetname, sizeof(targetname));

	if( strcmp(targetname, TRIPMINE_TARGETNAME) == 0 )
	{
		// Release the timers
		ITimer *timer = GET_TRIPMINE_TIMER(entity);

		if( timer != nullptr )
		{
			timersys->KillTimer(timer); SET_TRIPMINE_TIMER(entity, nullptr);
		}
	}

	return true;
}

void TripmineItem::OnPostRoundStart()
{
/*
	ZIPlayer *player = nullptr;	
	CBaseEntity *tripmineEnt = nullptr;

	for( auto iterator = ZICore::m_pOnlinePlayers.begin(); iterator != ZICore::m_pOnlinePlayers.end(); iterator++ )
	{
		player = *iterator;

		if( !player )
		{
			continue;
		}

		if( player->m_pTripmines.size() > 0 )
		{
			for( auto iterator2 = player->m_pTripmines.begin(); iterator2 != player->m_pTripmines.end(); iterator2++ )
			{
				tripmineEnt = *iterator2;

				if( !tripmineEnt )
				{
					continue;
				}

				AcceptEntityInput(tripmineEnt, "Kill");
			}

			player->m_pTripmines.clear();
		}

		player->m_HasTripmine = false;
	}
*/
}

void TripmineItem::OnPostClientCommand(ZIPlayer *player, const CCommand &args)
{
	const char *cmd = args[0];
	int *playerIndex = &player->m_Index;

	if( strcmp(cmd, CMD_PLANT_NAME) == 0 )
	{
		if( !player->m_IsAlive || player->m_IsInfected )
		{
			return;
		}

		if( !player->m_HasTripmine )
		{
			UM_SayText(playerIndex, 1, 0, true, "You don't have a \x04Tripmine\x01.");
			return;
		}

		if( g_TripmineItem.LookupForTripmine(player, false) )
		{
			player->m_pEntity->SetProgressBarTime(TRIPMINE_PLANT_DELAY);

			RELEASE_TIMER(player->m_pTripminePlantingTimer);
			player->m_pTripminePlantingTimer = timersys->CreateTimer(&m_TripminePlantingTimerCallback, TRIPMINE_PLANT_DELAY, player, TIMER_FLAG_NO_MAPCHANGE);
		}
		else
		{
			UM_SayText(playerIndex, 1, 0, true, "You need to look for a \x05close\x01 wall to plant your \x04Tripmine\x01.");
		}
	}
	else if( strcmp(cmd, CMD_TAKE_NAME) == 0 )
	{
		if( !player->m_IsAlive || player->m_IsInfected )
		{
			return;
		}

		if( player->m_HasTripmine && player->m_pTripmines.size() > 0 )
		{
			UM_SayText(playerIndex, 1, 0, true, "You can only hold one \x04Tripmine\x01 at \x05once\x01.");
			return;
		}

		BaseEntity *tripmineEnt = g_TripmineItem.LookupForTripmine(player, true);

		if( tripmineEnt )
		{
			player->m_pEntity->SetProgressBarTime(TRIPMINE_TAKE_DELAY);

			RELEASE_TIMER(player->m_pTripmineTakingTimer);
			player->m_pTripmineTakingTimer = timersys->CreateTimer(&m_TripmineTakingTimerCallback, TRIPMINE_TAKE_DELAY, new TripmineData(player, tripmineEnt), TIMER_FLAG_NO_MAPCHANGE);
		}
	}
}

void TripmineItem::OnPostClientInfection(ZIPlayer *player)
{
	// The mines will still exist, but can't take any rewards from them anymore
	if( player->m_pTripmines.size() > 0 )
	{
		BaseEntity *tripmineEnt = nullptr;

		for( auto iterator = player->m_pTripmines.begin(); iterator != player->m_pTripmines.end(); iterator++ )
		{
			tripmineEnt = *iterator;

			if( !tripmineEnt )
			{
				continue;
			}

			SET_TRIPMINE_OWNER(tripmineEnt, nullptr);
		}

		player->m_pTripmines.clear();
	}

	player->m_HasTripmine = false;
}

void TripmineItem::OnPostClientDeath(ZIPlayer *player)
{
	player->m_HasTripmine = false;	
}

int TripmineItem::OnPreMineTakeDamage(CTakeDamageInfo2 &info)
{
	BaseEntity *tripmineEnt = (BaseEntity *) META_IFACEPTR(CBaseEntity);
	
	ZIPlayer *attacker = ZIPlayer::Find(gamehelpers->ReferenceToIndex(info.GetAttacker()), false);

	if( !attacker )
	{
		RETURN_META_VALUE(MRES_SUPERCEDE, 0);
	}

	BasePlayer *ownerEnt = GET_TRIPMINE_OWNER(tripmineEnt);	
	BaseProp *glowEnt = GET_TRIPMINE_GLOW(tripmineEnt);

	if( !attacker->m_IsInfected && attacker->m_pEntity != ownerEnt && ownerEnt )
	{
		info.ScaleDamage(0.3);
	}

	float damage = info.GetDamage();
	int healthLeft = tripmineEnt->GetHealth() - damage;

	if( healthLeft <= 0 )
	{
		Vector center = tripmineEnt->GetOrigin();

		CellRecipientFilter filter;
		TE_Explosion(filter, 0.0f, center, m_ExplosionModelIndex, 8.5f, 15, 0, Vector(0.0f, 0.0f, 1.0f), 'C', TRIPMINE_EXPLOSION_RADIUS, 1000);

		const char *sound = ZIResources::GetRandomExplosionSound();

		CellRecipientFilter filter2;
		g_pExtension->m_pEngineSound->EmitSound(filter2, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) tripmineEnt)), CHAN_STATIC, sound, -1, sound, VOL_NORM, ATTN_NORM, 0);

		ZIPlayer *nearby = nullptr;
		BasePlayer *nearbyEnt = nullptr;
		int *nearbyIndex = nullptr;

		Vector origin, velocity;
		float distance = 0.0f, distanceRatio = 0.0f;

		// FindEntityInSphere is not implemented!! (need to find signature, use entitylist.cpp for help)
		while( (nearbyEnt = (BasePlayer *) BaseEntity::FindEntityInSphere(nearbyEnt, "player", center, TRIPMINE_EXPLOSION_RADIUS)) )
		{
			nearby = ZIPlayer::Find(nearbyEnt);

			if( !nearby || !nearby->m_IsAlive )
			{
				continue;
			}

			origin = nearbyEnt->GetOrigin();
			velocity = nearbyEnt->GetVelocity();

			distance = VectorLength(origin - center);
			distanceRatio = 1 - distance / TRIPMINE_EXPLOSION_RADIUS;
			
			velocity.x += (RandomInt(0, 1) == 0 ? RandomFloat(250.0f, 400.0f) : RandomFloat(-250.0f, -400.0f)) * distanceRatio;
			velocity.y += (RandomInt(0, 1) == 0 ? RandomFloat(250.0f, 400.0f) : RandomFloat(-250.0f, -400.0f)) * distanceRatio;
			velocity.z += (RandomFloat(200.0f, 300.0f)) * distanceRatio;

			nearbyEnt->Teleport(nullptr, nullptr, &velocity);
			nearbyIndex = &nearby->m_Index;

			if( nearby->m_IsInfected )
			{				
				nearbyEnt->TakeDamage(CTakeDamageInfo2((CBaseEntity *) tripmineEnt, (CBaseEntity *) ownerEnt, TRIPMINE_EXPLOSION_DAMAGE * distanceRatio, DMG_BLAST, (CBaseEntity *) tripmineEnt, Vector(0.0f, 0.0f, 0.0f), center));

				UM_ScreenFade(nearbyIndex, 1, RandomInt(600, 1000), 0, FFADE_IN, Color(255, 0, 0, 255));
				UM_ScreenShake(nearbyIndex, 1, SHAKE_START, RandomFloat(10.0f, 15.0f), RandomFloat(1.0f, 5.0f), RandomFloat(0.8f, 1.3f));

				nearby->PlaySound("ZombieInfestation/flatline.mp3");
			}		
		}

		// Remove the glow
		if( glowEnt )
		{
			glowEnt->AcceptInput("Kill");
		}

		// Release the timers
		ITimer *timer = GET_TRIPMINE_TIMER(tripmineEnt);

		if( timer != nullptr )
		{
			timersys->KillTimer(timer); SET_TRIPMINE_TIMER(tripmineEnt, nullptr);
		}
	}
	else if( glowEnt )
	{
		float healthRatio = damage / TRIPMINE_HEALTH;

		// DO NOT START FROM 255.0 OR 150.0 FOR ALPHA, IT'LL CAUSE THE UNSIGNED BE GIVEN A NEGATIVE VALUE, 
		//		AND IT'LL FLIP THE BITS....
		float colorShift = 225.0f * healthRatio;
		float alphaShift = 100.0f * healthRatio;

		Color color = glowEnt->GetGlowColor();

		color[0] += colorShift;
		color[1] -= colorShift;
		color[3] -= alphaShift;		

		glowEnt->SetGlowColor(color);
	}

	RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, 1, TripmineTakeDamage, (info));
}

void TripmineItem::CommandCallback(const CCommand &args)
{
}

ResultType TripmineItem::TripminePlantingTimerCallback::OnTimer(ITimer *timer, void *data)
{
	ZIPlayer *player = (ZIPlayer *) data;

	if( !player )
	{
		return Pl_Continue;
	}

	BasePlayer *playerEnt = player->m_pEntity;
	playerEnt->SetProgressBarTime(0);

	if( !player->m_IsAlive || player->m_IsInfected )
	{
		return Pl_Continue;
	}

	Vector dir;
	AngleVectors(playerEnt->GetEyeAngles(), &dir);

	Vector eyePos;
	g_pExtension->m_pServerGameClients->ClientEarPosition(player->m_pEdict, &eyePos);

	Ray_t ray;
	ray.Init(eyePos, eyePos + dir * TRIPMINE_MAX_DISTANCE);

	trace_t trace;
	CTraceFilterTripmines filter(player, false);

	g_pExtension->m_pEngineTrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

	Vector pos;

	if( trace.DidHit() )
	{
		dir = trace.plane.normal;
		pos = trace.endpos;
	}
	else
	{
		dir *= -1.0f;
		pos = eyePos - TRIPMINE_MAX_DISTANCE * dir;
	}

	BaseEntity *tripmineEnt = BaseEntity::CreateEntity("prop_physics_override");

	if( tripmineEnt )
	{
		QAngle angles;
		VectorAngles(dir, angles);

		tripmineEnt->SetKeyValue("targetname", TRIPMINE_TARGETNAME);
		tripmineEnt->SetKeyValue("model", "models/weapons/eminem/laser_mine/w_laser_mine_dropped3.mdl");
		tripmineEnt->SetKeyValue("solid", "6");
		tripmineEnt->SetKeyValue("origin", pos);
		tripmineEnt->SetKeyValue("angles", angles);

		SH_MANUAL_HOOK_CREATE(Tripmine_PreTakeDamage, tripmineEnt, SH_ADD_MANUALVPHOOK(TripmineTakeDamage, tripmineEnt, SH_MEMBER(&g_TripmineItem, &TripmineItem::OnPreMineTakeDamage), false));

		tripmineEnt->Spawn();

		tripmineEnt->SetCollisionGroup(COLLISION_GROUP_WEAPON);
		tripmineEnt->SetMoveType(MOVETYPE_NONE);
		tripmineEnt->SetHealth(TRIPMINE_HEALTH);
		tripmineEnt->SetTakeDamage(DAMAGE_NO);

		CellRecipientFilter filter;
		g_pExtension->m_pEngineSound->EmitSound(filter, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) tripmineEnt)), CHAN_ITEM, "ZombieInfestation/tripmine_deploy.mp3", -1, "ZombieInfestation/tripmine_deploy.mp3", VOL_NORM, ATTN_NORM, 0);

		CellRecipientFilter filter2;
		g_pExtension->m_pEngineSound->EmitSound(filter2, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) tripmineEnt)), CHAN_WEAPON, "ZombieInfestation/tripmine_charge.mp3", -1, "ZombieInfestation/tripmine_charge.mp3", VOL_NORM, ATTN_NORM, 0);

		SET_TRIPMINE_OWNER(tripmineEnt, player->m_pEntity);
		SET_TRIPMINE_TIMER(tripmineEnt, timersys->CreateTimer(&g_TripmineItem.m_TripmineThinkTimerCallback, 0.8f, tripmineEnt, TIMER_FLAG_NO_MAPCHANGE)); // TODO: Change this 			

		// One of his
		player->m_pTripmines.push_back(tripmineEnt);
	}

	player->m_HasTripmine = false;
	player->m_pTripminePlantingTimer = nullptr;

	return Pl_Stop;
}

void TripmineItem::TripminePlantingTimerCallback::OnTimerEnd(ITimer *timer, void *data)
{
}

ResultType TripmineItem::TripmineTakingTimerCallback::OnTimer(ITimer *timer, void *data)
{
	TripmineData *tripmineData = (TripmineData *) data;
	ZIPlayer *player = tripmineData->owner;

	if( !player )
	{
		return Pl_Continue;
	}

	player->m_pEntity->SetProgressBarTime(0);

	if( !player->m_IsAlive || player->m_IsInfected )
	{
		return Pl_Continue;
	}

	BaseEntity *tripmineEnt = tripmineData->entity;

	if( tripmineEnt )
	{
		tripmineEnt->AcceptInput("Kill");

		// Make sure we remove it from the entity list;
		player->m_pTripmines.remove(tripmineEnt);
	}

	delete tripmineData;

	player->m_HasTripmine = true;
	player->m_pTripmineTakingTimer = nullptr;

	return Pl_Stop;
}

void TripmineItem::TripmineTakingTimerCallback::OnTimerEnd(ITimer *timer, void *data)
{
}

ResultType TripmineItem::TripmineThinkTimerCallback::OnTimer(ITimer *timer, void *data)
{
	BaseEntity *tripmineEnt = (BaseEntity *) data;

	if( !tripmineEnt )
	{
		return Pl_Continue;
	}

	tripmineEnt->SetCollisionGroup(COLLISION_GROUP_NONE);
	tripmineEnt->SetTakeDamage(DAMAGE_YES);

	// Create some glow around it
	SET_TRIPMINE_GLOW(tripmineEnt, CreateEntityGlow(tripmineEnt, 0, Color(0, 255, 0, 150), nullptr));

	CellRecipientFilter filter;
	g_pExtension->m_pEngineSound->EmitSound(filter, gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference((CBaseEntity *) tripmineEnt)), CHAN_WEAPON, "ZombieInfestation/tripmine_activate.mp3", -1, "ZombieInfestation/tripmine_activate.mp3", VOL_NORM, ATTN_NORM, 0);

	return Pl_Continue;
}

void TripmineItem::TripmineThinkTimerCallback::OnTimerEnd(ITimer *timer, void *data)
{
}