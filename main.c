#include "pure-interface.h"

int main(int argc, char *argv[])
{
	scheduler_api_one();
	scheduler_api_two(10);

	pktio_api_one();
	pktio_api_two(10);
	return 0;
}
