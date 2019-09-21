#ifndef _INCLUDE_ZI_ENVIRONMENT_PROPER_H_
#define _INCLUDE_ZI_ENVIRONMENT_PROPER_H_
#pragma once

#include "zi_core.h"

namespace ZIEnvironment
{
	void ChangeSkybox();
	void ChangeLight();
	void ChangeWeather();

	void Setup();
	void RemoveUndesirableEnts(BaseEntity *entity, const char *classname);	
};

#endif 
