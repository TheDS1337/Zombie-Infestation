#ifndef _INCLUDE_ZI_GEOIP_PROPER_H_
#define _INCLUDE_ZI_GEOIP_PROPER_H_
#pragma once

#include "geoip/maxminddb.h"

namespace ZIGeoIP
{
	extern MMDB_s *m_pDataBase;

	// Taken from AMXx's GeoIP+ extension, geoip_utils.h/cpp files
	MMDB_entry_data_s *LookupByIP(const char *IP, const char **path);
	const char *LookupString(const char *IP, const char **path, int *length);
	double LookupDouble(const char *IP, const char **path);

	void Load();
	void Free();	

	bool GetCountry(const char *IP, char *output, int outputLen);
	bool GetCity(const char *IP, char *output, int outputLen);
};

#endif 