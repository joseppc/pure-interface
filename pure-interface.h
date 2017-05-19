#ifndef ODP_LINUX_H
#define ODP_LINUX_H

typedef enum { false = 0, true = 1 } bool;

extern const char *api_x(bool print);
extern const char *api_y(bool print);

extern void load_plugins(void);
extern void unload_plugins(void);

#endif

