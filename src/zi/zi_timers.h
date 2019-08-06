#ifndef _INCLUDE_ZI_TIMERS_PROPER_H_
#define _INCLUDE_ZI_TIMERS_PROPER_H_
#pragma once

#include "zi_core.h"

class ZITimersCallback
{
public:
	class Info final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static Info m_Info;

	class Warning final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static Warning m_Warning;

	class Countdown final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};
	
	static Countdown m_Countdown;

	class StartMode final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static StartMode m_StartMode;

	class AmbientSound final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static AmbientSound m_AmbientSound;

	class TeamsRandomization final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static TeamsRandomization m_TeamsRandomization;

	class BulletTime final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static BulletTime m_BulletTime;

	class Stats final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static Stats m_Stats;

	class SetModel final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};	
	
	static SetModel m_SetModel;

	class RemoveProtection final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static RemoveProtection m_RemoveProtection;

	class Respawn final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static Respawn m_Respawn;

	class ZombieGrowl final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static ZombieGrowl m_ZombieGrowl;

	class ZombieBleed final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};
	
	static ZombieBleed m_ZombieBleed;

	class ZombieUnfreeze final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	static ZombieUnfreeze m_ZombieUnfreeze;	
};

#endif 