AR := ar
CC := gcc
PWD := $(shell pwd)
CFLAGS := -Wall -Werror -g -I$(PWD) -I$(PWD)/framework

# The interface library compiles with modular framework and
# all subsystems and its own dynamic module loader (policies),
# while the modular framework only provides mechanisms to
# support variant dynamic module loader's implementation

INTERFACE_LIBRARY := libpure-interface.so

INTERFACE_SOURCES := module-loader.c       \
		     pktio-subsystem.c     \
		     scheduler-subsystem.c \
		     framework/module.c    \
		     framework/rwlock.c

$(INTERFACE_LIBRARY): $(patsubst %.c,%.po, $(INTERFACE_SOURCES))
	$(CC) $(CFLAGS) -shared -o $@ $^ -ldl

# The modules to implement subsystems are either built into
# static libraries or DSOs

SCHEDULER_DEFAULT := libscheduler-default.so

SCHEDULER_DEFAULT_OVERRIDE := libscheduler-default-override.so

SCHEDULER_DEFAULT_STATIC := libscheduler-default.a

SCHEDULER_DEFAULT_STATIC_OVERRIDE := libscheduler-default-override.a

SCHEDULER_DEFAULT_SOURCES := modules/scheduler-default.c

$(SCHEDULER_DEFAULT): $(patsubst %.c,%.po, $(SCHEDULER_DEFAULT_SOURCES))
	$(CC) $(CFLAGS) -shared -o $@ $^

$(SCHEDULER_DEFAULT_STATIC): $(patsubst %.c,%.o, $(SCHEDULER_DEFAULT_SOURCES))
	$(AR) rcs $@ $^

# IF scheduler default module is configured to override subsystem APIs.
$(SCHEDULER_DEFAULT_OVERRIDE): \
	$(patsubst %.c,%-override.po, $(SCHEDULER_DEFAULT_SOURCES))
	$(CC) $(CFLAGS) -shared -o $@ $^

$(SCHEDULER_DEFAULT_STATIC_OVERRIDE): \
	$(patsubst %.c,%-override.o, $(SCHEDULER_DEFAULT_SOURCES))
	$(AR) rcs $@ $^

%.E: %.c
	$(CC) $(CFLAGS) -E -o $@ $<

%.po: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

%-override.po: %.c %-override.h
	$(CC) $(CFLAGS) --include "$*-override.h" -fPIC -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%-override.o: %.c %-override.h
	$(CC) $(CFLAGS) --include "$*-override.h" -c -o $@ $<

main: main.o $(INTERFACE_LIBRARY) $(SCHEDULER_DEFAULT)
	$(CC) $< -L"${PWD}" -Wl,-R"${PWD}" -lpure-interface -o $@

main-static: main.o $(INTERFACE_LIBRARY) $(SCHEDULER_DEFAULT_STATIC)
	$(CC) $< -Wl,--whole-archive	\
	  $(SCHEDULER_DEFAULT_STATIC)	\
	  -Wl,--no-whole-archive	\
	  -L"${PWD}" -Wl,-R"${PWD}" -lpure-interface -o $@

main-static-override: main.o $(INTERFACE_LIBRARY) \
		      $(SCHEDULER_DEFAULT_STATIC_OVERRIDE)
	$(CC) $< -Wl,--whole-archive		\
	  $(SCHEDULER_DEFAULT_STATIC_OVERRIDE)	\
	  -Wl,--no-whole-archive		\
	  -L"${PWD}" -Wl,-R"${PWD}" -lpure-interface -o $@

clean:
	rm -f main main-static main-static-override
	find $(PWD) -regex ".*\.\(po\|so\|o\|E\|a\)$$" -delete

.PHONY: all clean
