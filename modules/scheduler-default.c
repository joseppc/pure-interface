#include <stdio.h>
#include <pure-interface.h>

/* Default scheduler module to implement the scheduler subsystem:
 * should work in both statically linked and DSO builds.
 */
static int default_api_one(void)
{
	printf("scheduler default: api_one().\n");
	return 0;
}

static const char *default_api_two(int input)
{
	printf("scheduler default: api_two(%d).\n", input);
	return NULL;
}

scheduler_module_t scheduler_default = {
	.name = "scheduler default",
	.init_local = NULL,
	.term_local = NULL,
	.init_global = NULL,
	.term_global = NULL,
	.api_one = default_api_one,
	.api_two = default_api_two,
};

MODULE_CONSTRUCTOR(scheduler_default)
{
	INIT_LIST_HEAD(&scheduler_default.list);

	printf("scheduler default module register.\n");
	subsystem_register_module(scheduler, &scheduler_default);
}
