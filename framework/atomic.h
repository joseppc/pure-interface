#ifndef ATOMIC_H
#define ATOMIC_H

#include <stdint.h>

/**
 * @internal
 * Atomic 32-bit unsigned integer
 */
struct atomic_u32_s {
	uint32_t v;
} __attribute__((aligned(sizeof(uint32_t))));

#define ATOMIC_INIT(a) { .v = a }

typedef struct atomic_u32_s atomic_u32_t;

static inline void atomic_init_u32(atomic_u32_t *atom, uint32_t val)
{
	__atomic_store_n(&atom->v, val, __ATOMIC_RELAXED);
}

static inline uint32_t atomic_load_u32(atomic_u32_t *atom)
{
	return __atomic_load_n(&atom->v, __ATOMIC_RELAXED);
}

static inline void atomic_store_u32(atomic_u32_t *atom, uint32_t val)
{
	__atomic_store_n(&atom->v, val, __ATOMIC_RELAXED);
}

static inline uint32_t atomic_fetch_add_u32(atomic_u32_t *atom,
					    uint32_t val)
{
	return __atomic_fetch_add(&atom->v, val, __ATOMIC_RELAXED);
}

static inline void atomic_add_u32(atomic_u32_t *atom, uint32_t val)
{
	(void)__atomic_fetch_add(&atom->v, val, __ATOMIC_RELAXED);
}

static inline uint32_t atomic_fetch_sub_u32(atomic_u32_t *atom,
					    uint32_t val)
{
	return __atomic_fetch_sub(&atom->v, val, __ATOMIC_RELAXED);
}

static inline void atomic_sub_u32(atomic_u32_t *atom, uint32_t val)
{
	(void)__atomic_fetch_sub(&atom->v, val, __ATOMIC_RELAXED);
}

static inline uint32_t atomic_fetch_inc_u32(atomic_u32_t *atom)
{
	return __atomic_fetch_add(&atom->v, 1, __ATOMIC_RELAXED);
}

static inline void atomic_inc_u32(atomic_u32_t *atom)
{
	(void)__atomic_fetch_add(&atom->v, 1, __ATOMIC_RELAXED);
}

static inline uint32_t atomic_fetch_dec_u32(atomic_u32_t *atom)
{
	return __atomic_fetch_sub(&atom->v, 1, __ATOMIC_RELAXED);
}

static inline void atomic_dec_u32(atomic_u32_t *atom)
{
	(void)__atomic_fetch_sub(&atom->v, 1, __ATOMIC_RELAXED);
}

static inline int atomic_cas_u32(atomic_u32_t *atom, uint32_t *old_val,
				 uint32_t new_val)
{
	return __atomic_compare_exchange_n(&atom->v, old_val, new_val,
					   0 /* strong */,
					   __ATOMIC_RELAXED,
					   __ATOMIC_RELAXED);
}

static inline uint32_t atomic_xchg_u32(atomic_u32_t *atom,
				       uint32_t new_val)
{
	return __atomic_exchange_n(&atom->v, new_val, __ATOMIC_RELAXED);
}

static inline void atomic_max_u32(atomic_u32_t *atom, uint32_t new_max)
{
	uint32_t old_val;

	old_val = atomic_load_u32(atom);

	while (new_max > old_val) {
		if (atomic_cas_u32(atom, &old_val, new_max))
			break;
	}
}

static inline void atomic_min_u32(atomic_u32_t *atom, uint32_t new_min)
{
	uint32_t old_val;

	old_val = atomic_load_u32(atom);

	while (new_min < old_val) {
		if (atomic_cas_u32(atom, &old_val, new_min))
			break;
	}
}

static inline uint32_t atomic_load_acq_u32(atomic_u32_t *atom)
{
	return __atomic_load_n(&atom->v, __ATOMIC_ACQUIRE);
}

static inline void atomic_store_rel_u32(atomic_u32_t *atom, uint32_t val)
{
	__atomic_store_n(&atom->v, val, __ATOMIC_RELEASE);
}

static inline void atomic_add_rel_u32(atomic_u32_t *atom, uint32_t val)
{
	(void)__atomic_fetch_add(&atom->v, val, __ATOMIC_RELEASE);
}

static inline void atomic_sub_rel_u32(atomic_u32_t *atom, uint32_t val)
{
	(void)__atomic_fetch_sub(&atom->v, val, __ATOMIC_RELEASE);
}

static inline int atomic_cas_acq_u32(atomic_u32_t *atom,
				     uint32_t *old_val, uint32_t new_val)
{
	return __atomic_compare_exchange_n(&atom->v, old_val, new_val,
					   0 /* strong */,
					   __ATOMIC_ACQUIRE,
					   __ATOMIC_RELAXED);
}

static inline int atomic_cas_rel_u32(atomic_u32_t *atom,
				     uint32_t *old_val, uint32_t new_val)
{
	return __atomic_compare_exchange_n(&atom->v, old_val, new_val,
					   0 /* strong */,
					   __ATOMIC_RELEASE,
					   __ATOMIC_RELAXED);
}

static inline int atomic_cas_acq_rel_u32(atomic_u32_t *atom,
					 uint32_t *old_val,
					 uint32_t new_val)
{
	return __atomic_compare_exchange_n(&atom->v, old_val, new_val,
					   0 /* strong */,
					   __ATOMIC_ACQ_REL,
					   __ATOMIC_RELAXED);
}

#endif
