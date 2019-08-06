#ifndef _INCLUDE_GAMERULES_PROPER_H_
#define _INCLUDE_GAMERULES_PROPER_H_
#pragma once

#include "extension.h"

enum CSGORoundEndReason;

extern void TerminateRound(float delay, CSGORoundEndReason reason);
extern int RoundTime();
extern float RoundStartTime();
extern bool CanCTBuy();

#endif 