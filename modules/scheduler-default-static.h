#ifndef SCHEDULER_DEFAULT_STATIC_H
#define SCHEDULER_DEFAULT_STATIC_H

#include "module.h"

/* The inclusion of this header file should be controlled by
 * Makefile system.
 */

extern int SUBSYSTEM_API_STATIC_OVERRIDE(
	scheduler, api_one, "default_api_one")(void);

extern const char *SUBSYSTEM_API_STATIC_OVERRIDE(
	scheduler, api_two, "default_api_two")(int);

#endif
