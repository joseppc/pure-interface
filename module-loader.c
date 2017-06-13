
#if 0
static void __attribute__((constructor)) load_plugins(void)
{
	printf("load api implementations.\n");

	plugins[0] = dlopen("libapi-x.so", RTLD_LAZY | RTLD_GLOBAL);
	if( NULL == plugins[0] )
		printf("cannot load libapi-x.so!\n");

	plugins[1] = dlopen("libapi-y.so", RTLD_LAZY | RTLD_GLOBAL);
	if( NULL == plugins[1] )
		printf("cannot load libapi-y.so!\n");

	api_protos.x = (api_x_proto)dlsym(plugins[0], "api_x");
	api_protos.y = (api_y_proto)dlsym(plugins[1], "api_y");
}

static void __attribute__((destructor)) unload_plugins(void)
{
	api_protos.x = NULL;
	api_protos.y = NULL;

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
#endif
