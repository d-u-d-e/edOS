/*
 * Copyright (c) 2011-2014, Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Macro utilities
 *
 * Macro utilities are the public interface for C/C++ code and device tree
 * related implementation.  In general, C/C++ will include <sys/util.h>
 * instead this file directly.  For device tree implementation, this file
 * should be include instead <sys/util_internal.h>
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Most of the eldritch implementation details for all the macrobatics
 * below (APIs like IS_ENABLED(), COND_CODE_1(), etc.) are hidden away
 * in this file.
 */
#include <os/include/util_internal.h>

#ifndef BIT
#if defined(_ASMLANGUAGE)
#define BIT(n)  (1 << (n))
#else
/**
 * @brief Unsigned integer with bit position @p n set (signed in
 * assembly language).
 */
#define BIT(n)  (1UL << (n))
#endif
#endif

/** @brief 64-bit unsigned integer with bit position @p _n set. */
#define BIT64(_n) (1ULL << (_n))

/**
 * @brief Set or clear a bit depending on a boolean value
 *
 * The argument @p var is a variable whose value is written to as a
 * side effect.
 *
 * @param var Variable to be altered
 * @param bit Bit number
 * @param set if 0, clears @p bit in @p var; any other value sets @p bit
 */
#define WRITE_BIT(var, bit, set) \
	((var) = (set) ? ((var) | BIT(bit)) : ((var) & ~BIT(bit)))

/**
 * @brief Bit mask with bits 0 through <tt>n-1</tt> (inclusive) set,
 * or 0 if @p n is 0.
 */
#define BIT_MASK(n) (BIT(n) - 1UL)

/**
 * @brief 64-bit bit mask with bits 0 through <tt>n-1</tt> (inclusive) set,
 * or 0 if @p n is 0.
 */
#define BIT64_MASK(n) (BIT64(n) - 1ULL)

/** @brief Check if a @p x is a power of two */
#define IS_POWER_OF_TWO(x) (((x) != 0U) && (((x) & ((x) - 1U)) == 0U))

/**
 * @brief Check if bits are set continuously from the specified bit
 *
 * The macro is not dependent on the bit-width.
 *
 * @param m Check whether the bits are set continuously or not.
 * @param s Specify the lowest bit for that is continuously set bits.
 */
#define IS_SHIFTED_BIT_MASK(m, s) (!(((m) >> (s)) & (((m) >> (s)) + 1U)))

/**
 * @brief Check if bits are set continuously from the LSB.
 *
 * @param m Check whether the bits are set continuously from LSB.
 */
#define IS_BIT_MASK(m) IS_SHIFTED_BIT_MASK(m, 0)

/* macros convert value of its argument to a string */
#define DO_TOSTR(s) #s
#define TOSTR(s) DO_TOSTR(s)

#define COND_CODE_1(_flag, _if_1_code, _else_code) \
	Z_COND_CODE_1(_flag, _if_1_code, _else_code)


/** @brief 0 if @p cond is true-ish; causes a compile error otherwise. */
#define ZERO_OR_COMPILE_ERROR(cond) ((int) sizeof(char[1 - 2 * !(cond)]) - 1)

/**
 * @brief Validate if two entities have a compatible type
 *
 * @param a the first entity to be compared
 * @param b the second entity to be compared
 * @return 1 if the two elements are compatible, 0 if they are not
 */
#define SAME_TYPE(a, b) __builtin_types_compatible_p(__typeof__(a), __typeof__(b))

/**
 * @brief Validate CONTAINER_OF parameters, only applies to C mode.
 */
#ifndef __cplusplus
#define CONTAINER_OF_VALIDATE(ptr, type, field)               \
	BUILD_ASSERT(SAME_TYPE(*(ptr), ((type *)0)->field) || \
		     SAME_TYPE(*(ptr), void),                 \
		     "pointer type mismatch in CONTAINER_OF");
#else
#define CONTAINER_OF_VALIDATE(ptr, type, field)
#endif

/**
 * @brief Get a pointer to a structure containing the element
 *
 * Example:
 *
 *	struct foo {
 *		int bar;
 *	};
 *
 *	struct foo my_foo;
 *	int *ptr = &my_foo.bar;
 *
 *	struct foo *container = CONTAINER_OF(ptr, struct foo, bar);
 *
 * Above, @p container points at @p my_foo.
 *
 * @param ptr pointer to a structure element
 * @param type name of the type that @p ptr is an element of
 * @param field the name of the field within the struct @p ptr points to
 * @return a pointer to the structure that contains @p ptr
 */
#define CONTAINER_OF(ptr, type, field)                               \
	({                                                           \
		CONTAINER_OF_VALIDATE(ptr, type, field)              \
		((type *)(((char *)(ptr)) - offsetof(type, field))); \
	})


#ifdef __cplusplus
}
#endif