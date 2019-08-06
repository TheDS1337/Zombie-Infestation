#ifndef _INCLUDE_ZI_SPAWNS_MANAGER_PROPER_H_
#define _INCLUDE_ZI_SPAWNS_MANAGER_PROPER_H_
#pragma once

#include "zi_core.h"

struct SpawnData
{
	Vector origin;
	QAngle angles;

	SpawnData()
	{
		origin = Vector(0.0f, 0.0f, 0.0f);
		angles = QAngle(0.0f, 0.0f, 0.0f);
	}
};

class ZISpawnsManager
{
public:
	static void Load();
	static void Free();	

	static bool Teleport(ZIPlayer *player);

	static SourceHook::CVector<SpawnData> m_SpawnData;
};

extern ConVar g_SpawnProtectionDuration;

extern ZISpawnsManager g_ZISpawnsManager;

#endif 