#include <stdio.h>
#include <errno.h>
#include "module.h"

/* This is tricky, dynamic module loader will install or
 * abandon the DSOs to finish the registration sessions.
 * But in statically linked modules situation, no one took
 * the responsibility to finish the bottom halves of the
 * registrations.
 */
static void (*static_post_registration)(subsystem_t *, void *)
	= &__subsystem_install_dso;

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

	/* We will not release the write lock, since this
	 * is only the top half of the registration. */
	rwlock_write_lock(&subsystem->lock);

	if (subsystem->registered != NULL) {
		module_base_t *prev =(module_base_t *)
			subsystem->registered;

		printf("A previous %s registration has "
		       "not finished yet.\n", prev->name);

		rwlock_write_unlock(&subsystem->lock);
		return -EAGAIN;
	}

	subsystem->registered = &module->list;

	if (static_post_registration != NULL)
		static_post_registration(subsystem, NULL);

	return 0;
}

void __subsystem_install_dso(subsystem_t *subsystem, void *dso)
{
	/* Bottom halves of the registration, mutal exclusion
	 * is guarenteed by subsystem_register_module() calls
	 */
	if (0 == rwlock_write_trylock(&subsystem->lock)) {
		module_base_t *module = (module_base_t *)
			subsystem->registered;

		if (module != NULL) {
			module->handler = dso;
			subsystem->registered = NULL;
			list_add(&subsystem->modules, &module->list);
		}
	}
	rwlock_write_unlock(&subsystem->lock);
}

void __subsystem_abandon_dso(subsystem_t *subsystem)
{
	/* Bottom halves of the registration, mutal exclusion
	 * is guarenteed by subsystem_register_module() calls
	 */
	if (0 == rwlock_write_trylock(&subsystem->lock)) {
		if (subsystem->registered != NULL)
			subsystem->registered = NULL;
	}
	rwlock_write_unlock(&subsystem->lock);
}

static void __attribute__((constructor(103)))
		static_post_registration_done(void)
{
	static_post_registration = NULL;
}
