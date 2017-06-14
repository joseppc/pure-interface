#include "pure-interface.h"

int main(int argc, char *argv[])
{
	dynamic_module_loader();

	scheduler_api_one();
	scheduler_api_two(10);

	pktio_api_one();
	pktio_api_two(10);
	return 0;
}
