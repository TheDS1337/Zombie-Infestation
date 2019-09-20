#ifndef _INCLUDE_ZI_ITEM_TRIPMINE_PROPER_H_
#define _INCLUDE_ZI_ITEM_TRIPMINE_PROPER_H_
#pragma once

#include "zi_items.h"

#define TRIPMINE_NAME "Tripmine"
#define TRIPMINE_VIP false
#define TRIPMINE_COST 1

#define TRIPMINE_TARGETNAME "zi_tripmine"
#define TRIPMINE_HEALTH 500
#define TRIPMINE_PLANT_DELAY 2.0f
#define TRIPMINE_TAKE_DELAY 5.0f
#define TRIPMINE_MAX_DISTANCE 80.0f
#define TRIPMINE_EXPLOSION_RADIUS 300.0f
#define TRIPMINE_EXPLOSION_DAMAGE RandomFloat(1000.0f, 2000.0f)

// I'll stock the owner into the entity's hacked offset, because setting the owner manually will make the tripmine SOLID_NONE to them....
#define GET_TRIPMINE_OWNER(tripmineEnt) *(BasePlayer **) ((char *) tripmineEnt + g_pExtension->m_OnUser4)
#define SET_TRIPMINE_OWNER(tripmineEnt, address) *(BasePlayer **) ((char *) tripmineEnt + g_pExtension->m_OnUser4) = address
#define GET_TRIPMINE_TIMER(tripmineEnt) *(ITimer **) ((char *) tripmineEnt + g_pExtension->m_OnUser3)
#define SET_TRIPMINE_TIMER(tripmineEnt, address) *(ITimer **) ((char *) tripmineEnt + g_pExtension->m_OnUser3) = address
#define GET_TRIPMINE_GLOW(tripmineEnt) *(BaseProp **) ((char *) tripmineEnt + g_pExtension->m_OnUser2)
#define SET_TRIPMINE_GLOW(tripmineEnt, address) *(BaseProp **) ((char *) tripmineEnt + g_pExtension->m_OnUser2) = address

struct TripmineData
{
	ZIPlayer *owner;
	BaseEntity *entity;
	BaseProp *glowEnt;

	TripmineData(ZIPlayer *owner, BaseEntity *entity)
	{
		this->owner = owner;
		this->entity = entity;
		this->glowEnt = nullptr;
	}

	~TripmineData()
	{
		this->owner = nullptr;
		this->entity = nullptr;
		this->glowEnt = nullptr;
	}
};

class TripmineItem final: public ZIItem, public ICommandCallback
{
private:
	ConCommand *m_pPlantTripmineCmd;
	ConCommand *m_pTakeTripmineCmd;

	BaseEntity *LookupForTripmine(ZIPlayer *player, bool take);

	void CommandCallback(const CCommand &command);

	class TripminePlantingTimerCallback final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	class TripmineTakingTimerCallback final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;		
		void OnTimerEnd(ITimer *timer, void *data) override;		
	};

	class TripmineThinkTimerCallback final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};	

	TripminePlantingTimerCallback m_TripminePlantingTimerCallback;
	TripmineTakingTimerCallback m_TripmineTakingTimerCallback;
	TripmineThinkTimerCallback m_TripmineThinkTimerCallback;

public:
	const char *GetName() override;
	bool IsVIP() override;
	int GetCost() override;

	ItemReturn OnPreSelection(ZIPlayer *player) override;
	void OnPostSelection(ZIPlayer *player) override;

	void OnLoad();
	void OnUnload();

	void Precache();
	void GetOffsets();

	void OnPostEntityCreation(BaseEntity *entity, const char *classname);
	bool OnPreEntityDestruction(BaseEntity *entity, const char *classname);
	void OnPostRoundStart();
	void OnPostClientCommand(ZIPlayer *player, const CCommand &args);	
	void OnPostClientInfection(ZIPlayer *player);
	void OnPostClientDeath(ZIPlayer *player);

	int OnPreMineTakeDamage(CTakeDamageInfo2 &info);	
};

extern TripmineItem g_TripmineItem;

#endif