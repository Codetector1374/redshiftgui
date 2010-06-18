/**\file		options.c
 * \author		Mao Yu
 * \brief		Options.
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "common.h"
#include "gamma.h"

/* Bounds for parameters. */
#define MIN_LAT   -90.0
#define MAX_LAT    90.0
#define MIN_LON  -180.0
#define MAX_LON   180.0
#define MIN_TEMP   1000
#define MAX_TEMP  10000
#define MIN_GAMMA   0.1
#define MAX_GAMMA  10.0

/* Default values for parameters. */
#define DEFAULT_DAY_TEMP    6500
#define DEFAULT_NIGHT_TEMP  3400
#define DEFAULT_GAMMA        1.0

/* Redshift options.*/
typedef struct{
	float lat;
	float lon;
	int temp_day;
	int temp_night;
	gamma_method_t method;
	int screen_num;
	gamma_s gamma;
	int crtc_num;
	int trans_speed;
	int one_shot;
	int nogui;
	int verbose;
} rs_opts;

static rs_opts Rs_opts;

int opt_get_config_file(char buffer[],size_t bufsize){
#ifndef _WIN32
		char *home = getenv("HOME");
#else
		char *home = getenv("APPDATA");
#endif
		size_t s_home = strlen(home);
		if( s_home < (bufsize+strlen(RSG_RCFILE)-5) ){
			strcpy(buffer,home);
			buffer[s_home++]=PATH_SEP;
			strcpy(buffer+s_home,RSG_RCFILE);
			return 1;
		}
		return 0;
}

// Load defaults
void opt_set_defaults(void){
	Rs_opts.lat = 0;
	Rs_opts.lon = 0;
	Rs_opts.temp_day = DEFAULT_DAY_TEMP;
	Rs_opts.temp_night = DEFAULT_NIGHT_TEMP;
	Rs_opts.gamma.r = DEFAULT_GAMMA;
	Rs_opts.gamma.g = DEFAULT_GAMMA;
	Rs_opts.gamma.b = DEFAULT_GAMMA;
	Rs_opts.method = -1;
	Rs_opts.screen_num = -1;
	Rs_opts.crtc_num = -1;
	Rs_opts.trans_speed = 100;
	Rs_opts.one_shot = 0;
	Rs_opts.nogui = 0;
	Rs_opts.verbose = 0;
}

// Sets the CRTC
int opt_set_crtc(int val){
	Rs_opts.crtc_num = val;
	return RET_FUN_SUCCESS;
}

// Sets the gamma
int opt_set_gamma(float r, float g, float b){
	Rs_opts.gamma.r = r;
	Rs_opts.gamma.g = g;
	Rs_opts.gamma.b = b;
	return RET_FUN_SUCCESS;
}

// Parses a gamma argument by the form of "R:G:B"
int opt_parse_gamma(char *val){
	char *s;
	s = strchr(val, ':');
	if (s == NULL) {
		/* Use value for all channels */
		float g = (float) atof(val);
		return opt_set_gamma(g,g,g);
	} else {
		char *gs;
		float r,g,b;
		/* Parse separate value for each channel */
		*(s++) = '\0';
		r = (float) atof(val); /* Red */
		gs = s;
		s = strchr(s, ':');
		if (s == NULL) {
			LOG(LOGERR,_("Malformed gamma argument: %s.\n"),val);
			return RET_FUN_FAILED;
		}
		*(s++) = '\0';
		g = (float) atof(gs); /* Green */
		b = (float) atof(s); /*  Blue */
		return opt_set_gamma(r,g,b);
	}
}

// Sets the location
int opt_set_location(float lat, float lon){
	Rs_opts.lat = lat;
	Rs_opts.lon = lon;
	return RET_FUN_SUCCESS;
}

// Parses the location argument in the form of "LAT:LON"
int opt_parse_location(char *val){
	char *s;
	float lat,lon;
	s = strchr(val, ':');
	if (s == NULL) {
		LOG(LOGERR,_("Malformed location argument: %s.\n"),val);
		return RET_FUN_FAILED;
	}
	*(s++) = '\0';
	lat = (float) atof(val);
	lon = (float) atof(s);
	return opt_set_location(lat,lon);
}

// Set no GUI mode
int opt_set_nogui(int val){
	Rs_opts.nogui = val;
	return RET_FUN_SUCCESS;
}

// Sets the method to change the monitor temperature
int opt_set_method(gamma_method_t method){
	Rs_opts.method = method;
	return RET_FUN_SUCCESS;
}

// Parses the method to change the monitor temperature
int opt_parse_method(char *val){
	int ret;
	if (strcmp(val, "randr") == 0 || strcmp(val, "RANDR") == 0) {
#ifdef ENABLE_RANDR
		ret = opt_set_method(GAMMA_METHOD_RANDR);
#else
		LOG(LOGERR,_("RANDR method was not enabled at compile time.\n"));
		ret = RET_FUN_FAILED;
#endif
	} else if (strcmp(val, "vidmode") == 0 ||strcmp(val, "VidMode") == 0) {
#ifdef ENABLE_VIDMODE
	    ret = opt_set_method(GAMMA_METHOD_VIDMODE);
#else
		LOG(LOGERR,_("VidMode method was not enabled at compile time.\n"));
		ret = RET_FUN_FAILED;
#endif
	} else if (strcmp(val, "wingdi") == 0 || strcmp(val, "WinGDI") == 0) {
#ifdef ENABLE_WINGDI
		ret = opt_set_method(GAMMA_METHOD_WINGDI);
#else
		LOG(LOGERR,_("WinGDI method was not enabled at compile time.\n"));
		ret = RET_FUN_FAILED;
#endif
	} else {
		/* TRANSLATORS: This refers to the method
		   used to adjust colors e.g VidMode */
		LOG(LOGERR,_("Unknown method `%s'.\n"),val);
		ret = RET_FUN_FAILED;
	}
	return ret;

}

// Sets oneshot mode
int opt_set_oneshot(int onoff){
	Rs_opts.one_shot = onoff;
	return RET_FUN_SUCCESS;
}

// Sets transition - change in temperature per second
int opt_set_transpeed(int tpersec){
	Rs_opts.trans_speed = tpersec;
	return RET_FUN_SUCCESS;
}

// Sets the screen to apply adjustment to
int opt_set_screen(int val){
	Rs_opts.screen_num = val;
	return RET_FUN_SUCCESS;
}

// Sets the temperature for day/night
int opt_set_temperatures(int tday, int tnight){
	Rs_opts.temp_day = tday;
	Rs_opts.temp_night = tnight;
	return RET_FUN_SUCCESS;
}

// Parses the transition temperature string in the form of "DAY:NIGHT"
int opt_parse_temperatures(char *val){
	char *s = strchr(val, ':');
	int temp_day, temp_night;
	if (s == NULL) {
		LOG(LOGERR,_("Malformed temperature argument: %s.\n"),val);
		return RET_FUN_FAILED;
	}
	*(s++) = '\0';
	temp_day = atoi(val);
	temp_night = atoi(s);
	return opt_set_temperatures(temp_day, temp_night);
}

// Sets the verbosity of logging
int opt_set_verbose(int level){
	Rs_opts.verbose = level;
	return RET_FUN_SUCCESS;
}

int opt_get_crtc(void)
{return Rs_opts.crtc_num;}

gamma_s opt_get_gamma(void)
{return Rs_opts.gamma;}

float opt_get_lat(void)
{return Rs_opts.lat;}

float opt_get_lon(void)
{return Rs_opts.lon;}

int opt_get_nogui(void)
{return Rs_opts.nogui;}

gamma_method_t opt_get_method(void)
{return Rs_opts.method;}

int opt_get_oneshot(void)
{return Rs_opts.one_shot;}

int opt_get_trans_speed(void)
{return Rs_opts.trans_speed;}

int opt_get_screen(void)
{return Rs_opts.screen_num;}

int opt_get_temp_day(void)
{return Rs_opts.temp_day;}

int opt_get_temp_night(void)
{return Rs_opts.temp_night;}

int opt_get_verbosity(void)
{return Rs_opts.verbose;}

