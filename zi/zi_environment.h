#ifndef _INCLUDE_ZI_ENVIRONMENT_PROPER_H_
#define _INCLUDE_ZI_ENVIRONMENT_PROPER_H_
#pragma once

#include "zi_core.h"

class ZIEnvironment
{
private:
	static void ChangeSkybox();
	static void ChangeLight();
	static void ChangeWeather();

public:
	static void Setup();
	static void RemoveUndesirableEnts(BaseEntity *entity, const char *classname);	
};

extern string_t LIGHT_STYLE;

#endif 
