#ifndef _INCLUDE_ZI_SOURCEMOD_BRIDGE_PROPER_H_
#define _INCLUDE_ZI_SOURCEMOD_BRIDGE_PROPER_H_
#pragma once

#include "zi_core.h"

namespace ZISourceModBridge
{
	void Load();
	void Free();

	extern IForward *m_pPrePlayerInfection;
	extern IForward	*m_pPostPlayerInfection;
	extern IForward *m_pPrePlayerDisinfection;
	extern IForward *m_pPostPlayerDisinfection;
	extern IForward *m_pPlayerLastHuman;
	extern IForward *m_pPlayerLastZombie;
	extern IForward *m_pPreItemSelection;
	extern IForward *m_pPostItemSelection;
	extern IForward *m_pRoundModeStart;
	extern IForward *m_pRoundModeEnd;
};

#endif 