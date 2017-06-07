AR := ar
CC := gcc
PWD := $(shell pwd)
CFLAGS := -Wall -Werror -g -I$(PWD) -I$(PWD)/framework

INTERFACE_LIBRARY := libpure-interface.so

INTERFACE_SOURCES := pure-interface.c      \
		     pktio-subsystem.c     \
		     scheduler-subsystem.c \
		     framework/rwlock.c

INTERFACE_OBJECTS := \
	$(patsubst %.c,%.po, $(INTERFACE_SOURCES))

$(INTERFACE_LIBRARY): $(INTERFACE_OBJECTS)
	$(CC) $(CFLAGS) -shared -o $@ $^ -ldl

SCHEDULER_DEFAULT_MODULE := libscheduler-default.a

SCHEDULER_DEFAULT_SOURCES := modules/scheduler-default.c

SCHEDULER_DEFAULT_OBJECTS := \
	$(patsubst %.c,%.o, $(SCHEDULER_DEFAULT_SOURCES))

# scheduler default module is built into static library
# and thus include the static header file to override subsystem APIs.
modules/scheduler-default.o: modules/scheduler-default.c \
			     modules/scheduler-default-static.h
	$(CC) $(CFLAGS) --include modules/scheduler-default-static.h -c -o $@ $<

$(SCHEDULER_DEFAULT_MODULE): $(SCHEDULER_DEFAULT_OBJECTS)
	$(AR) rcs $@ $^

ALL_MODULES := $(SCHEDULER_DEFAULT_MODULE)

%.E: %.c
	$(CC) $(CFLAGS) -fPIC -E -o $@ $<

%.po: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main: main.o $(INTERFACE_LIBRARY) $(ALL_MODULES)
	$(CC) $< -Wl,-Bstatic -lscheduler-default \
		-Wl,-Bdynamic -L"${PWD}" -Wl,-R"${PWD}" -lpure-interface \
		-o main

clean:
	rm -f main
	find $(PWD) -regex ".*\.\(po\|so\|o\|E\|a\)$$" -delete

.PHONY: all clean
