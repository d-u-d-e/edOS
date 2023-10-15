#pragma once

#if !defined(_ASMLANGUAGE)

/* Default definitions when no architecture specific definitions exist. */

/* Per CPU architecture specifics (empty) */
struct _cpu_arch {
#ifdef __cplusplus
	/* This struct will have a size 0 in C which is not allowed in C++ (it'll have a size 1). To
	 * prevent this, we add a 1 byte dummy variable.
	 */
	uint8_t dummy;
#endif
};

/* typedefs to be used with GEN_OFFSET_SYM(), etc. */
typedef struct _cpu_arch _cpu_arch_t;

#endif