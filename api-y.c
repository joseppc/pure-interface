#include <stdio.h>
#include "pure-interface.h"

const char *api_y(bool print)
{
	const char *name = "real api_y()";

	if (print) {
		printf("in %s.\n", name);
		printf("call %s.\n", api_x(false));
	}

	return name;
}

