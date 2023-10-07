#pragma once

/* ARM AArch32 GCC specific public inline assembler functions and macros */

#ifndef _ASMLANGUAGE

#include <kernel/include/types.h>
#include <kernel/include/util_macro.h>

#if defined(CONFIG_CPU_AARCH32_CORTEX_A)
#include <arch/arm/cortex-a/cpu.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* On ARMv7-M and ARMv8-M Mainline CPUs, this function prevents regular
 * exceptions (i.e. with interrupt priority lower than or equal to
 * _EXC_IRQ_DEFAULT_PRIO) from interrupting the CPU. NMI, Faults, SVC,
 * and Zero Latency IRQs (if supported) may still interrupt the CPU.
 *
 * On ARMv6-M and ARMv8-M Baseline CPUs, this function reads the value of
 * PRIMASK which shows if interrupts are enabled, then disables all interrupts
 * except NMI.
 */

static ALWAYS_INLINE unsigned int arch_irq_lock(void)
{
	unsigned int key;

#if defined(CONFIG_ARMV7_A)
	__asm__ volatile(
		"mrs %0, cpsr;"
		"and %0, #" TOSTR(I_BIT) ";"
		"cpsid i;"
		: "=r" (key)
		:
		: "memory", "cc");
#else
#error Unknown ARM architecture
#endif
	return key;
}

static ALWAYS_INLINE void arch_irq_unlock(unsigned int key)
{
#if defined(CONFIG_ARMV7_A)
	if (key != 0U) {
		return;
	}
	__asm__ volatile(
		"cpsie i;"
		: : : "memory", "cc");
#else
#error Unknown ARM architecture
#endif
}

static ALWAYS_INLINE bool arch_irq_unlocked(unsigned int key)
{
	return key == 0U;
}

static ALWAYS_INLINE void arch_nop(void)
{
	__asm__ volatile("nop");
}

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */