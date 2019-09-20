#ifndef _INCLUDE_ZI_RESOURCES_PROPER_H_
#define _INCLUDE_ZI_RESOURCES_PROPER_H_
#pragma once

#include "zi_core.h"
#include "zi_humans.h"

#define SOUNDS_PATH_MAXLENGTH 128
#define MODELS_PATH_MAXLENGTH 128
#define MATERIALS_PATH_MAXLENGTH 128

#define STRLEN_SOUND 6	// Length of "sound/"

enum RoundEndSound
{
	RoundEndSound_None = -1,
	RoundEndSound_Humans,
	RoundEndSound_Zombies,
	RoundEndSound_Max
};

enum ModelType
{
	ModelType_Human = 0,
	ModelType_Zombie,
	ModelType_Survivor,
	ModelType_Sniper,
	ModelType_Nemesis,
	ModelType_Assassin,
	ModelType_Max
};

class ZIModel
{
private:
	char m_Name[32];	
	char m_PlayerModel[MODELS_PATH_MAXLENGTH];
	char m_ArmsModel[MODELS_PATH_MAXLENGTH];
	int m_PlayerModelIndex;
	int m_ArmsModelIndex;

public:
	ZIModel();
	ZIModel(const char *name, const char *playermodel, const char *armsmodel, int playermodelIndex, int armsModelIndex);

	const char *GetPlayerModel(int *playermodelIndex = 0);
	const char *GetArmsModel(int *armsModelIndex = 0);

	friend class ZIResources;
};

class ZIResources
{
public:
	static int m_BloodSprayModelIndex;
	static int m_BloodDropModelIndex;

	static int m_LaserModelIndex;
	static int m_BeamModelIndex;
	static int m_HaloModelIndex;
	static int m_GlowModelIndex;

	static int m_FireModelIndex;
	static int m_ExplosionModelIndex;

	static int m_GlassModelIndex[6];

	static void Load();
	static void Free();

	static char *RandomLoadingSound();
	static char *RandomRoundEndSound(RoundEndSound sound);
	static const char *GetRandomExplosionSound();

	static ZIModel *GetModel(const char *name, ModelType type);
	static ZIModel *RandomModel(ModelType type);
};

extern ZIResources g_Resources;

#endif 