/* solar.h -- Solar position header
   This file is part of Redshift.

   Redshift is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Redshift is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Redshift.  If not, see <http://www.gnu.org/licenses/>.

   Copyright (c) 2010  Jon Lund Steffensen <jonlst@gmail.com>
*/

#ifndef _SOLAR_H
#define _SOLAR_H

#include "time.h"

/* Model of atmospheric refraction near horizon (in degrees). */
#define SOLAR_ATM_REFRAC  0.833

#define SOLAR_ASTRO_TWILIGHT_ELEV   -18.0f
#define SOLAR_NAUT_TWILIGHT_ELEV    -12.0f
#define SOLAR_CIVIL_TWILIGHT_ELEV    -6.0f
#define SOLAR_DAYTIME_ELEV           (0.0f - SOLAR_ATM_REFRAC)

typedef enum {
	SOLAR_TIME_NOON = 0,
	SOLAR_TIME_MIDNIGHT,
	SOLAR_TIME_ASTRO_DAWN,
	SOLAR_TIME_NAUT_DAWN,
	SOLAR_TIME_CIVIL_DAWN,
	SOLAR_TIME_SUNRISE,
	SOLAR_TIME_SUNSET,
	SOLAR_TIME_CIVIL_DUSK,
	SOLAR_TIME_NAUT_DUSK,
	SOLAR_TIME_ASTRO_DUSK,
	SOLAR_TIME_MAX
} solar_time_t;


double solar_elevation(double date, double lat, double lon);
void solar_table_fill(double date, double lat, double lon, double *table);

#endif /* ! _SOLAR_H */
