#include <stdio.h>
#include "pure-interface.h"

/* Subsystem pktio demonstrates mutliple implementations support
 * in dynamic loadable module forms, and subsystem APIs can choose
 * to route the calls to either implementation in runtime.
 */
SUBSYSTEM(pktio);

int SUBSYSTEM_API_STUB(pktio, api_one)(void)
{
	int result = 0;
	pktio_module_t *module = NULL;

	printf("pktio stub: api_one().\n");
	subsystem_read_lock(pktio);

	/* pktio API one only route to the active
	 * implementation. */
	module = (pktio_module_t *)subsystem(pktio).active;

	if (module != NULL && module->api_one != NULL)
		result = module->api_one();

	subsystem_read_unlock(pktio);
	return result;
}

const char *SUBSYSTEM_API_STUB(pktio, api_two)(int input)
{
	const char *result = NULL;
	pktio_module_t *module = NULL;

	printf("pktio stub: api_two(%d).\n", input);
	subsystem_read_lock(pktio);

	/* pktio API two iterates all registered modules
	 * until the implementation returns not-NULL result. */
	subsystem_foreach_module(pktio, module) {
		if (module != NULL && module->api_two != NULL) {
			result = module->api_two(input);
			if (result != NULL)
				break;
		}
	}

	subsystem_read_unlock(pktio);
	return result;
}

SUBSYSTEM_CONSTRUCTOR(pktio)
{
	subsystem_constructor(pktio);

	/* Further initialization per subsystem */
	printf("pktio subsystem constructor.\n");
}
