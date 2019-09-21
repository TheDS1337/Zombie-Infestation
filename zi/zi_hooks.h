#ifndef _INCLUDE_ZI_HOOKS_PROPER_H_
#define _INCLUDE_ZI_HOOKS_PROPER_H_
#pragma once

#include "extension.h"
#include "entity_hooker.h"

struct HookReturn
{
	META_RES ret;
	bool modified;

	HookReturn()
	{
		ret = MRES_IGNORED;
		modified = false;
	};

	HookReturn(META_RES ret, bool modified)
	{
		this->ret = ret;
		this->modified = modified;
	};
};

template<class Type> struct HookReturnValue
{
	META_RES ret;
	Type typeret;
	bool modified;

	HookReturnValue()
	{
		ret = MRES_IGNORED;		
		typeret = 0;
		modified = false;
	};

	HookReturnValue(META_RES ret, Type typeret, bool modified)
	{
		this->ret = ret;
		this->typeret = typeret;
		this->modified = modified;
	};
};

#define HOOK_CHECK_RETURN_VOID(hook) latestValue = hook; \
								 if( latestValue.ret > value.ret || (latestValue.modified && !value.modified) ) \
								 { \
									 value = latestValue; \
								 } \

#define HOOK_CHECK_RETURN_VALUE(hook) latestValue = hook; \
								  if( latestValue.ret > value.ret || latestValue.typeret > value.typeret || (latestValue.modified && !value.modified) ) \
								  { \
									  value = latestValue; \
								  } \

#define DETOUR_CHECK_RETURN(hook) latestValue = hook; \
								  if( latestValue > value ) \
								  { \
									  value = latestValue; \
								  } \

#define HOOK_RETURN_VOID(ret, modified)	return HookReturn(ret, modified)
#define HOOK_RETURN_VALUE(ret, type, typeret, modified)	return HookReturnValue<type>(ret, typeret, modified)

class ZIUserMessagesCallback
{
public:
	class VGUIMenu final: public IProtobufUserMessageListener
	{
	public:
		void OnUserMessage(int msgId, google::protobuf::Message *msg, IRecipientFilter *filter) override;		
		ResultType InterceptUserMessage(int msgId, google::protobuf::Message *msg, IRecipientFilter *filter) override;
	};

	VGUIMenu m_VGUIMenu;
};

namespace ZIHooks
{
	extern int m_VGUIMenuMsg;
	extern ZIUserMessagesCallback m_UserMessagesCallback;

	void SetupDetours();
	void ReleaseDetours();

	void GetOffsets();

	void AttachToServer();
	void AttachToClient(BasePlayer *clientEnt);
	void AttachToWeapon(BaseWeapon *weaponEnt);
	
	void OnPostProjectileCreation(BaseGrenade *nadeEnt, const char *classname);
	bool OnPreProjectileDestruction(BaseGrenade *nadeEnt, const char *classname);
	
	void Release();	
};

extern CDetour *g_pTerminateRoundDetour;

#endif 