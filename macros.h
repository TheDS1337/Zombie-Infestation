#ifndef _INCLUDE_MACROS_PROPER_H_
#define _INCLUDE_MACROS_PROPER_H_
#pragma once

#include "extension.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define TICK_INTERVAL2	g_pExtension->m_pGlobals->interval_per_tick
#define TIME_TO_TICKS2(dt) (int) (0.5f + (float) (dt / TICK_INTERVAL2))

#define CONSOLE_DEBUGGER ke::SafeSprintf(g_DebugInfoBuffer, 80, "%s - %s (line %d)", __FILE__, __FUNCTION__, __LINE__); DoDebug

#define LOOKUP_FOR_CONFIG(var, name) if( !gameconfs->LoadGameConfigFile(name, &var, buffer, sizeof(buffer)) ) \
									 { \
										 ke::SafeSprintf(error, maxlength, "Error loading " name ": %s", buffer); \
										 return false; \
									 }

#define CLOSE_CONFIG(var) if( var ) \
						  { \
							  gameconfs->CloseGameConfigFile(var); \
						  }

#define PRECACHE_SOUND(sound) AddFileToDownloadsTable("sound/" sound); \
							  g_pExtension->m_pEngineSound->PrecacheSound(sound, true)

#define PRECACHE_MODEL(model) AddFileToDownloadsTable(model); \
							  g_pExtension->m_pEngineServer->PrecacheModel(model, true);

#define GET_DATAMAP_OFFSET(offset) if( offset < 1 ) \
								   { \
                                       if( gamehelpers->FindDataMapInfo(datamap, #offset, &info) ) \
								       { \
									       offset = info.actual_offset; \
									   }  \
								       else \
									   { \
										   gotOffsets = false; \
									   } \
                                   }

#define GET_SENDPROP_OFFSET(serverclass, offset) if( offset < 1 ) \
												 { \
												     if( gamehelpers->FindSendPropInfo(#serverclass, #offset, &info) ) \
												     { \
													     offset = info.actual_offset;\
												     } \
												     else \
													 { \
														 gotOffsets = false; \
													 } \
                                                 }

#define GET_GAMERULES_OFFSET(offset) if( offset < 1 ) \
                                     { \
								         if( gamehelpers->FindSendPropInfo("CCSGameRulesProxy", #offset, &info) ) \
									     { \
										     offset = info.actual_offset;\
									     } \
									     else \
									     { \
										     gotOffsets = false; \
                                         } \
                                     }
 
#define SH_MANUAL_HOOK_CREATE(name, entity, hookId) EntityHook *name ## Hook = EntityHook::CreateHookIfNotFound(entity, #name, hookId); \
											if( !name ## Hook ) \
											{ \
												CONSOLE_DEBUGGER("Cannot create " #name " hook on entity: %d", entity); \
											}	

#define SH_MANUAL_HOOK_GET_OFFSET(name, key)	if( g_pExtension->m_pHooksConfig->GetOffset(key, &offset) ) \
												{ \
													SH_MANUALHOOK_RECONFIGURE(name, offset, 0, 0); \
												} \
												else if( g_pExtension->m_pSDKConfig->GetOffset(key, &offset) ) \
												{ \
													SH_MANUALHOOK_RECONFIGURE(name, offset, 0, 0); \
												} \
												else if( g_pExtension->m_pCStrikeConfig->GetOffset(key, &offset) ) \
												{ \
													SH_MANUALHOOK_RECONFIGURE(name, offset, 0, 0); \
												} \
												else if( g_ZombieInfestation.m_pConfig->GetOffset(key, &offset) ) \
												{ \
													SH_MANUALHOOK_RECONFIGURE(name, offset, 0, 0); \
												} \
												else \
												{ \
													CONSOLE_DEBUGGER("Offset for " key " was not found!"); \
												}

#define DETOUR_HOOK_DECLARE(name) CDetour *g_p ## name ## Detour = nullptr;

#define DETOUR_HOOK_CREATE_STATIC(name, key) if( g_pExtension->m_pHooksConfig->GetMemSig(key, &address) && address ) \
											 { \
												 g_p ## name ## Detour = DETOUR_CREATE_STATIC(name, address); \
												 if( g_p ## name ## Detour ) \
												 { \
													 g_p ## name ## Detour->EnableDetour(); \
												 } \
											 } \
											 else if( g_pExtension->m_pSDKConfig->GetMemSig(key, &address) && address ) \
											 { \
												 g_p ## name ## Detour = DETOUR_CREATE_STATIC(name, address); \
												 if( g_p ## name ## Detour ) \
												 { \
													 g_p ## name ## Detour->EnableDetour(); \
												 } \
											 } \
											 else if( g_pExtension->m_pCStrikeConfig->GetMemSig(key, &address) && address ) \
											 { \
												 g_p ## name ## Detour = DETOUR_CREATE_STATIC(name, address); \
												 if( g_p ## name ## Detour ) \
												 { \
													 g_p ## name ## Detour->EnableDetour(); \
												 } \
											 } \
											 else if( g_ZombieInfestation.m_pConfig->GetMemSig(key, &address) && address ) \
											 { \
												 g_p ## name ## Detour = DETOUR_CREATE_STATIC(name, address); \
												 if( g_p ## name ## Detour ) \
												 { \
													 g_p ## name ## Detour->EnableDetour(); \
												 } \
											 } \
											 else \
											 { \
												CONSOLE_DEBUGGER("Signature for " key " was not found!"); \
											 }

#define DETOUR_HOOK_CREATE_MEMBER(name, key) if( g_pExtension->m_pHooksConfig->GetMemSig(key, &address) && address ) \
											 { \
												 g_p ## name ## Detour = DETOUR_CREATE_MEMBER(name, address); \
												 if( g_p ## name ## Detour ) \
												 { \
													  g_p ## name ## Detour->EnableDetour(); \
												 } \
                                                 CONSOLE_DEBUGGER("Found signature for " key ""); \
											 } \
											 else if( g_pExtension->m_pSDKConfig->GetMemSig(key, &address) && address ) \
											 { \
												 g_p ## name ## Detour = DETOUR_CREATE_MEMBER(name, address); \
												 if( g_p ## name ## Detour ) \
												 { \
													  g_p ## name ## Detour->EnableDetour(); \
												 } \
                                                 CONSOLE_DEBUGGER("Found signature for " key ""); \
											 } \
											 else if( g_pExtension->m_pCStrikeConfig->GetMemSig(key, &address) && address ) \
											 { \
												 g_p ## name ## Detour = DETOUR_CREATE_MEMBER(name, address); \
												 if( g_p ## name ## Detour ) \
												 { \
													  g_p ## name ## Detour->EnableDetour(); \
												 } \
                                                 CONSOLE_DEBUGGER("Found signature for " key ""); \
											 } \
											 else if( g_ZombieInfestation.m_pConfig->GetMemSig(key, &address) && address ) \
											 { \
												 g_p ## name ## Detour = DETOUR_CREATE_MEMBER(name, address); \
												 if( g_p ## name ## Detour ) \
												 { \
													  g_p ## name ## Detour->EnableDetour(); \
												 } \
                                                 CONSOLE_DEBUGGER("Found signature for " key ""); \
											 } \
											 else \
											 { \
												 CONSOLE_DEBUGGER("Signature for " key " was not found!"); \
											 }

#define DETOUR_HOOK_RELEASE(name) if( g_p ## name ## Detour ) \
								  { \
									  g_p ## name ## Detour->Destroy(); g_p ## name ## Detour = nullptr; \
								  }

#define REGISTER_PRE_EVENT(name, callback) if( strcmp(eventName, name) == 0 ) \
										   { \
										       returnValue = callback(event, metaResult); \
										   }
#define REGISTER_POST_EVENT(name, callback) if( strcmp(eventName, name) == 0 ) \
											{ \
										        callback(event); \
											}

#define REGISTER_PRE_COMMAND(name, callback) if( strstr(command, name) ) \
											 { \
										         return callback(player, args); \
											 }

#define REGISTER_POST_COMMAND(name, callback) if( strstr(command, name) ) \
											  { \
										          callback(player, args); \
											  }

#define REGISTER_ARMS_MODEL(path) new ArmsModel(path)
#define REGISTER_PLAYER_MODEL(model, name, playermodel, armsmodel, flags) \
								model = new CModel(name, playermodel, armsmodel, flags); \
								if( model ) \
								{ \
									g_pPlayerModels.push_back(model); model = nullptr; \
								}

#define REGISTER_WEAPON(weapon, name, classname, allowedTeam, requiredLevel) weapon = new CWeapon(name, classname, allowedTeam, requiredLevel); \
																  if( weapon ) \
																  { \
																	  g_pAllowedWeapons.push_back(weapon); weapon = nullptr; \
																  }

#define RELEASE_CALL_WRAPPERS(wrappers) if( wrappers.size() > 0 ) \
										{ \
											for( auto wrapper = wrappers.begin(); wrapper != wrappers.end(); wrapper++ ) \
											{ \
												(*wrapper)->Destroy(); \
											} \
											wrappers.clear(); \
										} 


#define RELEASE_POINTERS_ARRAY(list) if( list.size() > 0 ) \
									 { \
										 for( auto i = list.begin(); i != list.end(); i++ ) \
										 { \
											 delete (*i); \
										 } \
										 list.clear(); \
									 }

#define RELEASE_POINTERS_ARRAY_ARRAY(list) if( list.size() > 0 ) \
										   { \
											   for( auto i = list.begin(); i != list.end(); i++ ) \
											   { \
												   delete[](*i); \
											   } \
											   list.clear(); \
										   }

#define RELEASE_MENU(menu) if( menu ) \
						   { \
						       menu->Destroy(false); menu = nullptr; \
						   }
#define CLOSE_MENU(menu) if( menu ) \
						   { \
						       menu->Cancel(); \
						   }

#define RELEASE_TIMER(timer) if( timer ) \
							 { \
								 timersys->KillTimer(timer); timer = nullptr; \
							 }

extern char g_DebugInfoBuffer[80];
extern void DoDebug(const char *format, ...);

extern void FireEventToClient(int clientIndex, IGameEvent *event);

extern string_t AllocPooledString(const char *str);

extern int FindCharInString(const char *str, char c, bool reverse = false);
extern unsigned int ReplaceStringsInString(char *subject, size_t maxlength, const char *search, const char *replace, bool caseSensitive = true);
extern char *ReplaceStringsInStringEx(char *subject, size_t maxLen, const char *search, size_t searchLen, const char *replace, size_t replaceLen, bool caseSensitive = true);

extern int AddCommas(int iNum, char *szOutput, int iLen);	// By Buggsy
extern int AddCommas(float num, char *output, int len);

#endif 
