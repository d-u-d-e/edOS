#pragma once

#include <arch/cpu.h>
#include <os/include/fatal_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fatal error policy handler
 *
 * This function is not invoked by application code, but is declared as a
 * weak symbol so that applications may introduce their own policy.
 *
 * The default implementation of this function halts the system
 * unconditionally. Depending on architecture support, this may be
 * a simple infinite loop, power off the hardware, or exit an emulator.
 *
 * If this function returns, then the currently executing thread will be
 * aborted.
 *
 * A few notes for custom implementations:
 *
 * - If the error is determined to be unrecoverable, LOG_PANIC() should be
 *   invoked to flush any pending logging buffers.
 * - K_ERR_KERNEL_PANIC indicates a severe unrecoverable error in the kernel
 *   itself, and should not be considered recoverable. There is an assertion
 *   in z_fatal_error() to enforce this.
 * - Even outside of a kernel panic, unless the fault occurred in user mode,
 *   the kernel itself may be in an inconsistent state, with API calls to
 *   kernel objects possibly exhibiting undefined behavior or triggering
 *   another exception.
 *
 * @param reason The reason for the fatal error
 * @param esf Exception context, with details and partial or full register
 *            state when the error occurred. May in some cases be NULL.
 */
void k_sys_fatal_error_handler(unsigned int reason, const z_arch_esf_t *esf);

/**
 * Called by architecture code upon a fatal error.
 *
 * This function dumps out architecture-agnostic information about the error
 * and then makes a policy decision on what to do by invoking
 * k_sys_fatal_error_handler().
 *
 * On architectures where k_thread_abort() never returns, this function
 * never returns either.
 *
 * @param reason The reason for the fatal error
 * @param esf Exception context, with details and partial or full register
 *            state when the error occurred. May in some cases be NULL.
 */
void z_fatal_error(unsigned int reason, const z_arch_esf_t *esf);


#ifdef __cplusplus
}
#endif
