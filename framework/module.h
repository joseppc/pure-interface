/**
 * @file
 *
 * Modular programming framework to support runtime selectable
 * implementations for variant software subsystems.
 *
 * Multiple implementations of the same subsystem can be built
 * into individual static libraries or loadable DSOs, and use
 * constructor functions to register themselves.
 *
 * A subsystem can choose one active implementation and provide
 * APIs to switch between implementations.
 *
 * Alternatively, subsystem can load multiple implementations
 * and determine the APIs route in runtime.
 *
 * this framework tries to minimizes dependencies to the linked
 * list and rwlock facilities only.
 */

#ifndef MODULE_H
#define MODULE_H

#include "list.h"
#include "rwlock.h"

typedef struct {
	rwlock_t lock;
	struct list_head modules, *registered;
	struct list_head *active;
} subsystem_t;

/* Subsystem instance name */
#define subsystem(name) name ##_## subsystem

/* Subsystem instantiation */
#define SUBSYSTEM(name) subsystem_t name ##_## subsystem

/* Subsystem API prototype name */
#define api_proto(subsystem, api) subsystem ##_## api ##_## proto_t

/* Subsystem API declaration */
#define SUBSYSTEM_API(name, _return, api, ...) 			\
	extern _return name ##_## api(__VA_ARGS__);		\
	typedef _return (*name ##_## api ##_## proto_t)		\
			(__VA_ARGS__)

/* Subsystem API stubs are weak */
#define SUBSYSTEM_API_STUB(name, api) 				\
	__attribute__((weak)) name ##_## api

/* In case subsystem API implementations are built as static
 * libraries or preload DSOs, one implementation can use this
 * macro to override the APIs weak stubs.
 */
#define SUBSYSTEM_API_OVERRIDE(name, api, _alias)		\
	__attribute__((alias(#_alias))) name ##_## api

#define subsystem_constructor(name) 				\
	do {							\
		rwlock_init(&name ##_## subsystem.lock);	\
		INIT_LIST_HEAD(&name ##_## subsystem.modules);	\
		name ##_## subsystem.registered = NULL;		\
		name ##_## subsystem.active = NULL;		\
	} while(0)

#define SUBSYSTEM_CONSTRUCTOR(name) 				\
	static void __attribute__((constructor(101)))		\
		name ##_## subsystem ##_## constructor(void)

#define subsystem_lock(access, name)				\
	rwlock_ ##access## _lock(&name ##_## subsystem.lock)

#define subsystem_unlock(access, name)				\
	rwlock_ ##access## _unlock(&name ##_## subsystem.lock)

#define subsystem_foreach_module(name, mod)			\
	list_for_each_entry(mod, &name ##_## subsystem.modules, list)

#define MODULE_CLASS(subsystem)					\
	struct subsystem ##_## module {				\
		struct list_head list;				\
		void *handler; /* DSO */			\
		const char *name;				\
		int (*init_local)(void);			\
		int (*term_local)(void);			\
		int (*init_global)(void);			\
		int (*term_global)(void);			\

/* Base class to all inherited subsystem module classes */
typedef MODULE_CLASS(base) } module_base_t;

/* Module constructors should be late than subsystem constructors,
 * in statically linked scenarios (both subsystems and modules are
 * linked statically). thus the priority 102 compared to the above
 * subsystem constructor priority 101.
 */
#define MODULE_CONSTRUCTOR(name) 				\
	static void __attribute__((constructor(102)))		\
		name ##_## module ##_## constructor(void)

/* Subsystem modules registration:
 * The purpose of this complicated routine aims to help dynamic
 * module loader to properly handle dlopen() failures as well as
 * to save the DSO handlers into subsystem modules' data structure
 * instead of maintain a separate storage of DSO handlers dis-
 * connected with the subsystem modules.
 *
 * The dynamic module loader should program in this way:
 *	module_loader_start();
 *	...any policies as read configurations or...
 *	...predefined names to decide the DSOs to be loaded...
 * 	for each DSO
 *		dlopen(DSO)
 *		-- the module constructor calls register_module()
 *		dlopen(DSO) returns the handler
 *		success ? install_dso() to submit the registration
 *		failure ? abandon_dso() to cancel the registration
 *	module_loader_end();
 */

extern void module_loader_start(void);
extern void module_loader_end(void);

extern int module_install_dso(void *);
extern int module_abandon_dso(void);

extern int __subsystem_register_module(subsystem_t *, module_base_t *);

/* Macro to allow polymorphism on module data structures */
#define subsystem_register_module(name, module)			  \
({								  \
	module_base_t *base = (module_base_t *)module;		  \
	__subsystem_register_module(&name ##_## subsystem, base); \
})

#endif
