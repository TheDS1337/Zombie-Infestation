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

namespace ZISpawnsManager
{
	void Load();
	void Free();	

	bool Teleport(ZIPlayer *player);

	extern SourceHook::CVector<SpawnData> m_SpawnData;
};

extern ConVar g_SpawnProtectionDuration;

#endif 