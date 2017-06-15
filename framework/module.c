#include <stdio.h>
#include <errno.h>
#include "module.h"

#define MODULE_FRAMEWORK_VERSION 0x00010000UL
SUBSYSTEM(module, "module framework", MODULE_FRAMEWORK_VERSION);

/* Keep it simple, allow one registration session at a time. */
typedef struct {
	rwlock_t lock;
	subsystem_t *subsystem;
	module_base_t *module;
} registration_session_t;

static registration_session_t registration = {
	.lock = RW_LOCK_UNLOCKED(lock),
	.subsystem = NULL,
	.module = NULL,
};

/* Module is linked statically or dynamically, and are loaded by
 * program loader (execve) or dynamic linker/loader (ld.so)
 *
 * subsystem_register_module() should complete the whole registration
 * session and link the module into subsystem's module array.
 */
static int linker_register_module(
	subsystem_t *subsystem, module_base_t *module)
{
	if (subsystem == NULL || module == NULL)
		return -ENOENT;

	if (!list_empty(&module->list)) {
		printf("module %s was already registered.\n",
		        module->name);
		return -EAGAIN;
	}

	rwlock_write_lock(&registration.lock);

	module->handler = NULL;
	rwlock_write_lock(&subsystem->lock);
	list_add(&module->list, &subsystem->modules);
	rwlock_write_unlock(&subsystem->lock);

	rwlock_write_unlock(&registration.lock);
	return 0;
}

static int (*do_register_module)(subsystem_t *, module_base_t *)
		= &linker_register_module;

static int loader_register_module(
	subsystem_t *subsystem, module_base_t *module)
{
	if (subsystem == NULL || module == NULL)
		return -ENOENT;

	if (!list_empty(&module->list)) {
		printf("module %s was already registered.\n",
		        module->name);
		return -EAGAIN;
	}

	if (rwlock_write_trylock(&registration.lock) == 0) {
		registration.subsystem = subsystem;
		registration.module = module;
		return 0;
	}

	rwlock_write_unlock(&registration.lock);
	return -EACCES;
}

void module_loader_start(void)
{
	rwlock_write_lock(&registration.lock);

	if (registration.module != NULL ||
	    registration.subsystem != NULL) {
		printf("module loader start warn, A previous "
		       "registration did not complete yet.\n");
	}

	registration.module = NULL;
	registration.subsystem = NULL;
	do_register_module = &loader_register_module;
}

void module_loader_end(void)
{
	if (registration.module != NULL ||
	    registration.subsystem != NULL) {
		printf("module loader end warn, A previous "
		       "registration did not complete yet.\n");
	}

	registration.module = NULL;
	registration.subsystem = NULL;
	do_register_module = &linker_register_module;

	rwlock_write_unlock(&registration.lock);
}

int module_install_dso(void *dso)
{
	/* Bottom halves of the registration, context exclusion
	 * is guarenteed by module_loader_start()
	 */
	if (0 == rwlock_write_trylock(&registration.lock)) {
		subsystem_t *subsystem = registration.subsystem;
		module_base_t *module = registration.module;

		if (subsystem != NULL && module != NULL) {
			module->handler = dso;
			rwlock_write_lock(&subsystem->lock);
			list_add(&module->list, &subsystem->modules);
			rwlock_write_unlock(&subsystem->lock);
		}

		registration.subsystem = NULL;
		registration.module = NULL;
		return 0;
	}

	rwlock_write_unlock(&registration.lock);
	return -EACCES;
}

int module_abandon_dso(void)
{
	/* Bottom halves of the registration, context exclusion
	 * is guarenteed by module_loader_start()
	 */
	if (0 == rwlock_write_trylock(&registration.lock)) {
		registration.subsystem = NULL;
		registration.module = NULL;
		return 0;
	}

	rwlock_write_unlock(&registration.lock);
	return -EACCES;
}

int __subsystem_register_module(
	subsystem_t *subsystem, module_base_t *module)
{
	return do_register_module(subsystem, module);
}
