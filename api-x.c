#include <stdio.h>
#include "pure-interface.h"

const char *api_x(bool print)
{
	const char *name = "real api_x()";

	if (print) {
		printf("in %s.\n", name);
		printf("call %s.\n", api_y(false));
	}

	return name;
}

