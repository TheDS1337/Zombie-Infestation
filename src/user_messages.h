#ifndef _INCLUDE_USER_MESSAGES_PROPER_H_
#define _INCLUDE_USER_MESSAGES_PROPER_H_
#pragma once

#include "extension.h"
#include "netmessages.pb.h"
#include "cstrike15_usermessage_helpers.h"
#include "cstrike15_usermessages.pb.h"

// Fade flags
#define FFADE_IN			0x0001		// Just here so we don't pass 0 into the function
#define FFADE_OUT			0x0002		// Fade out (not in)
#define FFADE_MODULATE		0x0004		// Modulate (don't blend)
#define FFADE_STAYOUT		0x0008		// ignores the duration, stays faded out until new ScreenFade message received
#define FFADE_PURGE			0x0010		// Purges all other fades, replacing them with this one

// Shake Commands
#define SHAKE_START         0			// Starts the screen shake for all players within the radius. 
#define SHAKE_STOP          1			// Stops the screen shake for all players within the radius. 
#define SHAKE_AMPLITUDE     2			// Modifies the amplitude of an active screen shake for all players within the radius. 
#define SHAKE_FREQUENCY     3			// Modifies the frequency of an active screen shake for all players within the radius. 
#define SHAKE_START_RUMBLEONLY 4		// Starts a shake effect that only rumbles the controller, no screen effect. 
#define SHAKE_START_NORUMBLE   5		// Starts a shake that does NOT rumble the controller. 

// TODO: resee all the usages of these
extern void UM_SayText(int *clients, int clientsCount, int author, bool addTag, const char *input);
extern void UM_HudText(int *clients, int clientsCount, int channel, float posX, float posY, Color color1, Color color2, int effect, float fadeInTime, float fadeOutTime, float holdTime, float fxTime, const char *input);

extern void UM_ScreenFade(int *clients, int clientsCount, int duration, int holdTime, int flags, Color color);
extern void UM_ScreenShake(int *clients, int clientsCount, int command, float amplitude, float frequency, float duration);

#endif 
