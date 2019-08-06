#ifndef _INCLUDE_STRINGTABLE_PROPER_H_
#define _INCLUDE_STRINGTABLE_PROPER_H_
#pragma once

#include "extension.h"

extern void FindStringTables();

extern void AddFileToDownloadsTable(const char *file);
extern void AddSoundToPrecacheTable(const char *sound);

extern const char *GetStringInTable(const char *tablename, int index);

extern int PrecacheParticleSystem(const char *name);
extern int PrecacheEffect(const char *name);

#endif