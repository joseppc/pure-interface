#include <stdio.h>
#include <errno.h>
#include "module.h"

/* This is tricky, dynamic module loader will install or abandon
 * the DSOs to finish the registration sessions. But in static or
 * dynamic linked scenarios, no dynamic moduler loader there to
 * complete the bottom halves of the registrations.
 */

static subsystem_t *__subsystem_in_registration = NULL;
static int (*__post_registration)(void *) = &module_install_dso;

void module_loader_start(void)
{
	__subsystem_in_registration = NULL;
	__post_registration = NULL;
}

void module_loader_end(void)
{
	__subsystem_in_registration = NULL;
	__post_registration = &module_install_dso;
}

/* This is called by every module in its module constructor, and
 * must accormodate all the flavors of modules builds as:
 * 1) static library and static linked
 * 2) DSO and dynamic linked (cc -llibrary)
 *    In both it should complete the whole registration process
 * 3) DSO and loaded by dynamic module loader (dlopen()ed)
 *    In above it partially hold the registered module until the
 *    dynamic module loader calls install_dso() or abandon_dso()
 */
int __subsystem_register_module(
	subsystem_t *subsystem, module_base_t *module)
{
	if (subsystem == NULL || module == NULL)
		return -EINVAL;

	if (!list_empty(&module->list)) {
		printf("module %s was already registered.\n",
		        module->name);
		return -EAGAIN;
	}

	/* Hold the write lock in top half of the registration */
	rwlock_write_lock(&subsystem->lock);

	if ((__subsystem_in_registration != NULL) ||
	    (subsystem->registered != NULL))
	{
		printf("A previous registration "
		       "has not been finished yet.\n");

		rwlock_write_unlock(&subsystem->lock);
		return -EAGAIN;
	}

	__subsystem_in_registration = subsystem;
	subsystem->registered = &module->list;

	if (__post_registration != NULL)
		return __post_registration(NULL);

	return 0;
}

int module_install_dso(void *dso)
{
	subsystem_t *subsystem =
		__subsystem_in_registration;

	if (subsystem == NULL)
		return -ENOENT;

	/* Bottom halves of the registration, mutal exclusion
	 * is guarenteed by subsystem_register_module() calls
	 */
	if (0 == rwlock_write_trylock(&subsystem->lock)) {
		if (subsystem->registered != NULL) {
			module_base_t *module = (module_base_t *)
				subsystem->registered;

			module->handler = dso;
			subsystem->registered = NULL;
			__subsystem_in_registration = NULL;
			list_add(&module->list, &subsystem->modules);
		}
	}

	rwlock_write_unlock(&subsystem->lock);
	return 0;
}

int module_abandon_dso(void)
{
	subsystem_t *subsystem =
		__subsystem_in_registration;

	if (subsystem == NULL)
		return 0;

	/* Bottom halves of the registration, mutal exclusion
	 * is guarenteed by subsystem_register_module() calls
	 */
	if (0 == rwlock_write_trylock(&subsystem->lock)) {
		if (subsystem->registered != NULL)
			subsystem->registered = NULL;
			__subsystem_in_registration = NULL;
	}

	rwlock_write_unlock(&subsystem->lock);
	return 0;
}
