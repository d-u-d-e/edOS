#pragma once

#include <os/include/kernel_includes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Wrapper around printk to avoid including printk.h in assert.h */
void __printf_like(1, 2) assert_print(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#ifdef ARCH_EXCEPT
/* This architecture has direct support for triggering a CPU exception */
#define z_except_reason(reason)	ARCH_EXCEPT(reason)
#else

#if defined(CONFIG_ASSERT_VERBOSE)
#define __ASSERT_PRINT(fmt, ...) assert_print(fmt, ##__VA_ARGS__)
#else /* CONFIG_ASSERT_VERBOSE */
#define __ASSERT_PRINT(fmt, ...)
#endif /* CONFIG_ASSERT_VERBOSE */

#if !defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __EXCEPT_LOC() __ASSERT_PRINT("@ %s:%d\n", __FILE__, __LINE__)
#else
#define __EXCEPT_LOC()
#endif

/* NOTE: This is the implementation for arches that do not implement
 * ARCH_EXCEPT() to generate a real CPU exception.
 *
 * We won't have a real exception frame to determine the PC value when
 * the oops occurred, so print file and line number before we jump into
 * the fatal error handler.
 */
#define z_except_reason(reason) do { \
		__EXCEPT_LOC();              \
		z_fatal_error(reason, NULL); \
	} while (false)

#endif /* _ARCH__EXCEPT */

/**
 * @brief Fatally terminate the system
 *
 * This should be called when the Ed kernel has encountered an
 * unrecoverable runtime condition and needs to terminate. What this ultimately
 * means is determined by the _fatal_error_handler() implementation, which
 * will be called with reason code K_ERR_KERNEL_PANIC.
 */
#define k_panic()	z_except_reason(K_ERR_KERNEL_PANIC)

#if CONFIG_NUM_COOP_PRIORITIES + CONFIG_NUM_PREEMPT_PRIORITIES == 0
#error Zero available thread priorities defined!
#endif

#define K_PRIO_COOP(x) (-(CONFIG_NUM_COOP_PRIORITIES - (x)))
#define K_PRIO_PREEMPT(x) (x)

#define K_HIGHEST_THREAD_PRIO (-CONFIG_NUM_COOP_PRIORITIES)
#define K_LOWEST_THREAD_PRIO CONFIG_NUM_PREEMPT_PRIORITIES
#define K_IDLE_PRIO K_LOWEST_THREAD_PRIO
#define K_HIGHEST_APPLICATION_THREAD_PRIO (K_HIGHEST_THREAD_PRIO)
#define K_LOWEST_APPLICATION_THREAD_PRIO (K_LOWEST_THREAD_PRIO - 1)

#ifdef CONFIG_MULTITHREADING
extern struct k_thread z_idle_threads[CONFIG_MP_MAX_NUM_CPUS];
#endif