#ifndef _INCLUDE_ZI_SOURCEMOD_BRIDGE_PROPER_H_
#define _INCLUDE_ZI_SOURCEMOD_BRIDGE_PROPER_H_
#pragma once

#include "zi_core.h"

class ZISourceModBridge
{
public:
	static void Load();
	static void Free();

	static IForward *m_pPrePlayerInfection;
	static IForward	*m_pPostPlayerInfection;
	static IForward *m_pPrePlayerDisinfection;
	static IForward *m_pPostPlayerDisinfection;
	static IForward *m_pPlayerLastHuman;
	static IForward *m_pPlayerLastZombie;
	static IForward *m_pPreItemSelection;
	static IForward *m_pPostItemSelection;
	static IForward *m_pRoundModeStart;
	static IForward *m_pRoundModeEnd;
};

extern ZISourceModBridge g_SourceModBridge;

#endif 