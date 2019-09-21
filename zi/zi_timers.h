#ifndef _INCLUDE_ZI_TIMERS_PROPER_H_
#define _INCLUDE_ZI_TIMERS_PROPER_H_
#pragma once

#include "zi_core.h"

namespace ZITimersCallback
{
	class Info final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern Info m_Info;

	class Warning final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern Warning m_Warning;

	class Countdown final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};
	
	extern Countdown m_Countdown;

	class StartMode final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern StartMode m_StartMode;

	class AmbientSound final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern AmbientSound m_AmbientSound;

	class TeamsRandomization final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern TeamsRandomization m_TeamsRandomization;

	class BulletTime final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern BulletTime m_BulletTime;

	class Stats final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern Stats m_Stats;

	class SetModel final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};	
	
	extern SetModel m_SetModel;

	class RemoveProtection final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern RemoveProtection m_RemoveProtection;

	class Respawn final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern Respawn m_Respawn;

	class ZombieGrowl final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern ZombieGrowl m_ZombieGrowl;

	class ZombieBleed final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};
	
	extern ZombieBleed m_ZombieBleed;

	class ZombieUnfreeze final: public ITimedEvent
	{
	public:
		ResultType OnTimer(ITimer *timer, void *data) override;
		void OnTimerEnd(ITimer *timer, void *data) override;
	};

	extern ZombieUnfreeze m_ZombieUnfreeze;	
};

#endif 