#include "gamerules.h"

void TerminateRound(float delay, CSGORoundEndReason reason)
{
#if defined _WINDOWS
	void *gamerules = g_pExtension->m_pSDKTools->GetGameRules();

	if( !gamerules )
	{
		CONSOLE_DEBUGGER("Couldn't find gamerules pointer");
		return;
	}

	static void *address = nullptr;
	if( !g_pGameMod->GetConfig()->GetMemSig("CCSGameRules::TerminateRound", &address) || !address )
	{
		CONSOLE_DEBUGGER("CCSGameRules::TerminateRound address is not found!");
		return;
	}
		
	__asm
	{
		push 0
		push 0
		push reason
		movss xmm1, delay
		mov ecx, gamerules

		call address
	}

#elif defined _LINUX	
	static ICallWrapper *callWrapper = nullptr;

	if( !callWrapper )
	{
		void *address = nullptr;
		if( !g_pGameMod->GetConfig()->GetMemSig("CCSGameRules::TerminateRound", &address) || !address )
		{
			CONSOLE_DEBUGGER("CCSGameRules::TerminateRound address is not found!");
			return;
		}

		PassInfo info[4];

		info[0].flags = PASSFLAG_BYVAL;
		info[0].size = sizeof(float);
		info[0].type = PassType_Float;
		info[1].flags = PASSFLAG_BYVAL;
		info[1].size = sizeof(CSGORoundEndReason);
		info[1].type = PassType_Basic;
		info[2].flags = PASSFLAG_BYVAL;
		info[2].size = sizeof(int);
		info[2].type = PassType_Basic;
		info[3].flags = PASSFLAG_BYVAL;
		info[3].size = sizeof(int);
		info[3].type = PassType_Basic;

		callWrapper = g_pExtension->m_pBinTools->CreateCall(address, CallConv_ThisCall, nullptr, info, 4);

		if( !callWrapper )
		{
			CONSOLE_DEBUGGER("CS:GO is not supported!");
			return;
		}

		g_pExtension->m_pBinCallWrappers.push_back(callWrapper);
	}

	unsigned char params[sizeof(void *) + sizeof(float) + sizeof(CSGORoundEndReason) + 2 * sizeof(int)];
	unsigned char *vparams = params;

	*(void **) vparams = g_pExtension->m_pGameRules; vparams += sizeof(void *);
	*(float *) vparams = delay; vparams += sizeof(float);
	*(CSGORoundEndReason *) vparams = reason; vparams += sizeof(CSGORoundEndReason);
	*(int *) vparams = 0; vparams += sizeof(int);
	*(int *) vparams = 0; 

	callWrapper->Execute(params, nullptr);
#endif
}

int RoundTime()
{
	void *gamerules = g_pExtension->m_pSDKTools->GetGameRules();

	if( !gamerules || !g_pExtension->m_pGameRulesProxyEnt )
	{
		CONSOLE_DEBUGGER("Unable to find GameRules pointer.");
		return false;
	}

	return *(int *) ((char *) gamerules + g_pExtension->m_iRoundTime);
}

float RoundStartTime()
{
	void *gamerules = g_pExtension->m_pSDKTools->GetGameRules();

	if( !gamerules || !g_pExtension->m_pGameRulesProxyEnt )
	{
		CONSOLE_DEBUGGER("Unable to find GameRules pointer.");
		return false;
	}

	return *(float *) ((char *) gamerules + g_pExtension->m_fRoundStartTime);
}

bool CanCTBuy()
{
	void *gamerules = g_pExtension->m_pSDKTools->GetGameRules();

	if( !gamerules || !g_pExtension->m_pGameRulesProxyEnt )
	{
		CONSOLE_DEBUGGER("Unable to find GameRules pointer.");
		return false;
	}

	return *(bool *) ((char *) gamerules + g_pExtension->m_bCTCantBuy) == false;
}