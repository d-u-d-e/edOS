#pragma once

#ifndef _ASMLANGUAGE

#include <os/include/types.h>

struct _callee_saved {
	uint32_t v1;  /* r4 */
	uint32_t v2;  /* r5 */
	uint32_t v3;  /* r6 */
	uint32_t v4;  /* r7 */
	uint32_t v5;  /* r8 */
	uint32_t v6;  /* r9 */
	uint32_t v7;  /* r10 */
	uint32_t v8;  /* r11 */
	uint32_t psp; /* r13 */
};

#if defined(CONFIG_FPU) && defined(CONFIG_FPU_SHARING)
struct _preempt_float {
	float  s16;
	float  s17;
	float  s18;
	float  s19;
	float  s20;
	float  s21;
	float  s22;
	float  s23;
	float  s24;
	float  s25;
	float  s26;
	float  s27;
	float  s28;
	float  s29;
	float  s30;
	float  s31;
};
#endif

typedef struct _callee_saved _callee_saved_t;


struct _thread_arch {

	/* interrupt locking key */
	uint32_t basepri;

#if defined(CONFIG_FPU) && defined(CONFIG_FPU_SHARING)
	/*
	 * No cooperative floating point register set structure exists for
	 * the Cortex-M as it automatically saves the necessary registers
	 * in its exception stack frame.
	 */
	struct _preempt_float  preempt_float;
#endif

};

typedef struct _thread_arch _thread_arch_t;

#endif