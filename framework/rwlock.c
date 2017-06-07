#include "rwlock.h"

void rwlock_init(rwlock_t *rwlock)
{
	atomic_init_u32(&rwlock->cnt, 0);
}

void rwlock_read_lock(rwlock_t *rwlock)
{
	uint32_t cnt;
	int  is_locked = 0;

	while (is_locked == 0) {
		cnt = atomic_load_u32(&rwlock->cnt);
		/* waiting for read lock */
		if ((int32_t)cnt < 0)
			continue;
		is_locked = atomic_cas_acq_u32(&rwlock->cnt,
					       &cnt, cnt + 1);
	}
}

int rwlock_read_trylock(rwlock_t *rwlock)
{
	uint32_t cnt = atomic_load_u32(&rwlock->cnt);

	while (cnt != (uint32_t)-1) {
		if (atomic_cas_acq_u32(&rwlock->cnt, &cnt, cnt + 1))
			return 1;
	}

	return 0;
}

void rwlock_read_unlock(rwlock_t *rwlock)
{
	atomic_sub_rel_u32(&rwlock->cnt, 1);
}

void rwlock_write_lock(rwlock_t *rwlock)
{
	uint32_t cnt;
	int is_locked = 0;

	while (is_locked == 0) {
		uint32_t zero = 0;
		cnt = atomic_load_u32(&rwlock->cnt);
		/* lock acquired, wait */
		if (cnt != 0)
			continue;
		is_locked = atomic_cas_acq_u32(&rwlock->cnt,
					       &zero, (uint32_t)-1);
	}
}

int rwlock_write_trylock(rwlock_t *rwlock)
{
	uint32_t zero = 0;

	return atomic_cas_acq_u32(&rwlock->cnt, &zero, (uint32_t)-1);
}

void rwlock_write_unlock(rwlock_t *rwlock)
{
	atomic_store_rel_u32(&rwlock->cnt, 0);
}
