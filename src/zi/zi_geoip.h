#ifndef _INCLUDE_ZI_GEOIP_PROPER_H_
#define _INCLUDE_ZI_GEOIP_PROPER_H_
#pragma once

#include "geoip/maxminddb.h"

class ZIGeoIP
{
private:
	static MMDB_s *m_pDataBase;

	// Taken from AMXx's GeoIP+ extension, geoip_utils.h/cpp files
	static MMDB_entry_data_s *LookupByIP(const char *IP, const char **path);
	static const char *LookupString(const char *IP, const char **path, int *length);
	static double LookupDouble(const char *IP, const char **path);

public:
	static void Load();
	static void Free();	

	static bool GetCountry(const char *IP, char *output, int outputLen);
	static bool GetCity(const char *IP, char *output, int outputLen);
};

extern ZIGeoIP g_GeoIP;

#endif 