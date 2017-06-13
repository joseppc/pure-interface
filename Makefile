AR := ar
CC := gcc
PWD := $(shell pwd)
CFLAGS := -Wall -Werror -g -I$(PWD) -I$(PWD)/framework

INTERFACE_LIBRARY := libpure-interface.so

INTERFACE_LIBRARY_STATIC := libpure-interface.a

INTERFACE_SOURCES := module-loader.c       \
		     pktio-subsystem.c     \
		     scheduler-subsystem.c \
		     framework/module.c    \
		     framework/rwlock.c

$(INTERFACE_LIBRARY): $(patsubst %.c,%.po, $(INTERFACE_SOURCES))
	$(CC) $(CFLAGS) -shared -o $@ $^ -ldl

$(INTERFACE_LIBRARY_STATIC): $(patsubst %.c,%.o, $(INTERFACE_SOURCES))
	$(AR) rcs $@ $^

SCHEDULER_DEFAULT := libscheduler-default.so

SCHEDULER_DEFAULT_STATIC := libscheduler-default.a

SCHEDULER_DEFAULT_SOURCES := modules/scheduler-default.c

$(SCHEDULER_DEFAULT): $(patsubst %.c,%.po, $(SCHEDULER_DEFAULT_SOURCES))
	$(CC) $(CFLAGS) -shared -o $@ $^

# Scheduler default module is built into static library
# and include the static header file to override subsystem APIs.
modules/scheduler-default.o: modules/scheduler-default.c \
			     modules/scheduler-default-override.h
	$(CC) $(CFLAGS) --include modules/scheduler-default-override.h -c -o $@ $<

$(SCHEDULER_DEFAULT_STATIC): $(patsubst %.c,%.o, $(SCHEDULER_DEFAULT_SOURCES))
	$(AR) rcs $@ $^

ALL_STATICS := $(INTERFACE_LIBRARY_STATIC) $(SCHEDULER_DEFAULT_STATIC)
ALL_DYNAMICS := $(INTERFACE_LIBRARY) $(SCHEDULER_DEFAULT)

%.E: %.c
	$(CC) $(CFLAGS) -fPIC -E -o $@ $<

%.po: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main: main.o $(ALL_DYNAMICS)
	$(CC) $< -L"${PWD}" -Wl,-R"${PWD}" -lpure-interface -o main

main-static: main.o $(ALL_STATICS)
	$(CC) $< -Wl,--whole-archive $(ALL_STATICS) -Wl,--no-whole-archive -o main-static

clean:
	rm -f main main-static
	find $(PWD) -regex ".*\.\(po\|so\|o\|E\|a\)$$" -delete

.PHONY: all clean
