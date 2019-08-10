#include "zi_spawns_manager.h"
#include "zi_players.h"
#include "zi_timers.h"

#define ZOMBIEINFESTATION_SPAWNS_FOLDER "addons/ZombieInfestation/spawns"

ConVar g_SpawnProtectionDuration("zi_spawn_protection_duration", "3.0");

SourceHook::CVector<SpawnData> ZISpawnsManager::m_SpawnData;

void ZISpawnsManager::Load()
{	
	const char *mapname = gamehelpers->GetCurrentMap();

	char path[128];
	g_pSM->BuildPath(Path_Game, path, sizeof(path), "%s/%s.ini", ZOMBIEINFESTATION_SPAWNS_FOLDER, mapname);

	if( !libsys->PathExists(path) || !libsys->IsPathFile(path) )
	{
		CONSOLE_DEBUGGER("No spawns data was found for the map: %s", mapname);
		return;
	}

	FILE *file = fopen(path, "rt");
	
	if( file )
	{
		float data[6] = { 0.0f };		
		char *dataEnd = nullptr;

		char line[256];
		int spaces = 0;

		// We devide the buffer into lines first
		while( !feof(file) && fgets(line, 256, file) )
		{
			dataEnd = line;	

			for( spaces = 0; spaces < 5; spaces++ )
			{
				data[spaces] = strtod(dataEnd, &dataEnd);
			}		

			// We need atleast the origin
			if( spaces >= 2 )
			{
				SpawnData spawn;
				spawn.origin = Vector(data[0], data[1], data[2]);

				// We push the angles only if we actually have some, otherwise it'll use the last iteration's angles
				if( spaces == 5 )
				{
					spawn.angles = QAngle(data[3], data[4], data[5]);
				}

				m_SpawnData.push_back(spawn);
			}			
		}
	}
}

void ZISpawnsManager::Free()
{
	m_SpawnData.clear();
}

bool ZISpawnsManager::Teleport(ZIPlayer *player)
{
	int size = m_SpawnData.size();

	if( size < 1 )
	{
		CONSOLE_DEBUGGER("Unable to find any spawn point.");
		return false;
	}
	
	if( !player || !player->m_IsAlive )
	{
		return false;
	}

	BasePlayer *playerEnt = player->m_pEntity;
	SpawnData *data = nullptr;

	while( (data = &m_SpawnData[RandomInt(0, size - 1)]) )
	{
		if( !playerEnt->CanGetInSpace(data->origin, playerEnt->GetFlags() & FL_DUCKING ? true : false) )
		{
			continue;
		}

		bool playerIsNearby = false;

		// Check if an enemy is nearby
		ZIPlayer *nearby = nullptr;
		BaseEntity *nearbyEnt = nullptr;

		// FindEntityInSphere is not implemented!! (need to find signature, use entitylist.cpp for help)
		while( (nearbyEnt = BaseEntity::FindEntityInSphere(nearbyEnt, "player", data->origin, 200.0f)) )
		{
			nearby = ZIPlayer::Find((BasePlayer *) nearbyEnt);

			if( !nearby )
			{
				continue;
			}

			playerIsNearby = true;
		}

		// Having nearby players could potentially make the player stuck, we avoid that
		if( !playerIsNearby )
		{
			playerEnt->Teleport(&data->origin, &data->angles, nullptr);
			return true;
		}
	}


	// Do some special protection, it's not faire to get attacked right after spawn...
	float protectionDuration = g_SpawnProtectionDuration.GetFloat();

	if( !player->m_IsInfected && protectionDuration > 0.0f )
	{
		player->m_IsProtected = true;

		RELEASE_TIMER(player->m_pRemoveProtectionTimer);
		player->m_pRemoveProtectionTimer = timersys->CreateTimer(&ZICore::m_TimersCallback.m_RemoveProtection, protectionDuration, player, TIMER_FLAG_NO_MAPCHANGE);
	}

	return false;
}