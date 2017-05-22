#include <stdio.h>
#include <dlfcn.h>
#include "pure-interface.h"

const char *__attribute__((weak)) api_x(bool print) 
{
	const char *name = "stub api_x()";

	if (print == true) {
		printf("in %s.\n", name);
		printf("call %s.\n", api_y(false));
	}

	return name;
}

const char *__attribute__((weak)) api_y(bool print)
{
	const char *name = "stub api_y()";

	if (print == true) {
		printf("in %s.\n", name);
		printf("call %s.\n", api_x(false));
	}

	return name;
}

static void *plugins[2] = { NULL, NULL };

static void __attribute__((constructor)) load_plugins(void)
{
	printf("load api implementations.\n");

	plugins[0] = dlopen("libapi-x.so", RTLD_LAZY | RTLD_GLOBAL);
	if( NULL == plugins[0] )
		printf("cannot load libapi-x.so!\n");

	plugins[1] = dlopen("libapi-y.so", RTLD_LAZY | RTLD_GLOBAL);
	if( NULL == plugins[1] )
		printf("cannot load libapi-y.so!\n");
}

static void __attribute__((destructor)) unload_plugins(void)
{
	if( NULL != plugins[0] ) {
		dlclose(plugins[0]);
		plugins[0] = NULL;
	}

	if( NULL != plugins[1] ) {
		dlclose(plugins[1]);
		plugins[1] = NULL;
	}

	printf("unload api implementations.\n");
}

