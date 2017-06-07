#include <stdio.h>
#include "pure-interface.h"

/* Subsystem scheduler demonstrates support for statically linked
 * implementation with API symbols replacement, to reduce overhead
 * of locks and API function pointers.
 */
SUBSYSTEM(scheduler);

int SUBSYSTEM_API_STUB(scheduler, api_one)(void)
{
	printf("scheduler stub: api_one().\n");
	return 0;
}

const char *SUBSYSTEM_API_STUB(scheduler, api_two)(int input)
{
	printf("scheduler stub: api_two(%d).\n", input);
	return NULL;
}

SUBSYSTEM_CONSTRUCTOR(scheduler)
{
	subsystem_constructor(scheduler);

	/* Further initialization per subsystem */
	printf("scheduler subsystem constructor.\n");
}
