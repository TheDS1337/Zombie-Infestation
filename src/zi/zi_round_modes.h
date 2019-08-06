#ifndef _INCLUDE_ZI_ROUND_MODES_PROPER_H_
#define _INCLUDE_ZI_ROUND_MODES_PROPER_H_
#pragma once

#include "zi_entry.h"

enum RoundModeWinner
{
	RoundModeWinner_Unknown = -1,
	RoundModeWinner_Humans,
	RoundModeWinner_Zombies,
	RoundModeWinner_Max
};

struct AmbientSoundInfo
{
	const char *sound;
	float duration;
};

class ZIRoundMode
{
private:
	int m_Index;

public:
	static int m_TotalChances;
	static ZIPlayer *m_RoundTarget;	

	static int Register(ZIRoundMode *mode);
	static ZIRoundMode *Find(const char *name);

	static ZIRoundMode *Choose();
	static void Start(ZIRoundMode *mode, ZIPlayer *target = nullptr);
	static ZIRoundMode *GetFromTotalChances(int chance);

	int GetIndex();

	virtual const char *GetName() = 0;
	virtual int GetChance() = 0;
	virtual bool IsInfectionAllowed() = 0;
	virtual const char *GetSound();
	virtual AmbientSoundInfo *GetAmbientSound();

	virtual bool OnPreSelection();
	virtual void OnPostSelection() = 0;
};

extern SourceHook::CVector<ZIRoundMode *> g_pRoundModes;

#endif 
