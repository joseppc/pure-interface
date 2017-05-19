#include "pure-interface.h"

int main(int argc, char *argv[])
{
	/* expects: stub implementations */
	api_x(true);
	api_y(true);

	load_plugins();

	/* expects: real implementations */
	api_x(true);
	api_y(true);

	unload_plugins();

	/* expects: restore stub implementaitons */
	api_x(true);
	api_y(true);

	return 0;
}

