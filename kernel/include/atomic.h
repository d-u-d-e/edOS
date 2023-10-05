#pragma once

#include <config/config.h>

#if defined(CONFIG_ATOMIC_OPERATIONS_C)
/* Generic-but-slow implementation based on kernel locking and syscalls */
#include <kernel/include/atomic_c.h>
#elif defined(CONFIG_ATOMIC_OPERATIONS_ARCH)
#include <kernel/include/atomic_arch.h>
#else
#include <kernel/include/atomic_builtin.h>
#endif

#include <kernel/include/util_macro.h>


#ifdef __cplusplus
extern "C" {
#endif


#define ATOMIC_BITS (sizeof(atomic_val_t) * 8)
#define ATOMIC_MASK(bit) BIT((unsigned long)(bit) & (ATOMIC_BITS - 1U))
#define ATOMIC_ELEM(addr, bit) ((addr) + ((bit) / ATOMIC_BITS))

/**
 * @brief Atomically test a bit.
 *
 * This routine tests whether bit number @a bit of @a target is set or not.
 * The target may be a single atomic variable or an array of them.
 *
 * @note As for all atomic APIs, includes a
 * full/sequentially-consistent memory barrier (where applicable).
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 *
 * @return true if the bit was set, false if it wasn't.
 */
static inline bool atomic_test_bit(const atomic_t *target, int bit)
{
	atomic_val_t val = atomic_get(ATOMIC_ELEM(target, bit));

	return (1 & (val >> (bit & (ATOMIC_BITS - 1)))) != 0;
}

/**
 * @brief Atomically test and clear a bit.
 *
 * Atomically clear bit number @a bit of @a target and return its old value.
 * The target may be a single atomic variable or an array of them.
 *
 * @note As for all atomic APIs, includes a
 * full/sequentially-consistent memory barrier (where applicable).
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 *
 * @return false if the bit was already cleared, true if it wasn't.
 */
static inline bool atomic_test_and_clear_bit(atomic_t *target, int bit)
{
	atomic_val_t mask = ATOMIC_MASK(bit);
	atomic_val_t old;

	old = atomic_and(ATOMIC_ELEM(target, bit), ~mask);

	return (old & mask) != 0;
}

/**
 * @brief Atomically set a bit.
 *
 * Atomically set bit number @a bit of @a target and return its old value.
 * The target may be a single atomic variable or an array of them.
 *
 * @note As for all atomic APIs, includes a
 * full/sequentially-consistent memory barrier (where applicable).
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 *
 * @return true if the bit was already set, false if it wasn't.
 */
static inline bool atomic_test_and_set_bit(atomic_t *target, int bit)
{
	atomic_val_t mask = ATOMIC_MASK(bit);
	atomic_val_t old;

	old = atomic_or(ATOMIC_ELEM(target, bit), mask);

	return (old & mask) != 0;
}

/**
 * @brief Atomically clear a bit.
 *
 * Atomically clear bit number @a bit of @a target.
 * The target may be a single atomic variable or an array of them.
 *
 * @note As for all atomic APIs, includes a
 * full/sequentially-consistent memory barrier (where applicable).
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 */
static inline void atomic_clear_bit(atomic_t *target, int bit)
{
	atomic_val_t mask = ATOMIC_MASK(bit);

	(void)atomic_and(ATOMIC_ELEM(target, bit), ~mask);
}


/**
 * @brief Atomically set a bit.
 *
 * Atomically set bit number @a bit of @a target.
 * The target may be a single atomic variable or an array of them.
 *
 * @note As for all atomic APIs, includes a
 * full/sequentially-consistent memory barrier (where applicable).
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 */
static inline void atomic_set_bit(atomic_t *target, int bit)
{
	atomic_val_t mask = ATOMIC_MASK(bit);

	(void)atomic_or(ATOMIC_ELEM(target, bit), mask);
}

/**
 * @brief Atomically set a bit to a given value.
 *
 * Atomically set bit number @a bit of @a target to value @a val.
 * The target may be a single atomic variable or an array of them.
 *
 * @note As for all atomic APIs, includes a
 * full/sequentially-consistent memory barrier (where applicable).
 *
 * @param target Address of atomic variable or array.
 * @param bit Bit number (starting from 0).
 * @param val true for 1, false for 0.
 */
static inline void atomic_set_bit_to(atomic_t *target, int bit, bool val)
{
	atomic_val_t mask = ATOMIC_MASK(bit);

	if (val) {
		(void)atomic_or(ATOMIC_ELEM(target, bit), mask);
	} else {
		(void)atomic_and(ATOMIC_ELEM(target, bit), ~mask);
	}
}

#ifdef __cplusplus
} /* extern "C" */
#endif