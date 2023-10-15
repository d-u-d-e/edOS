#include <os/include/fatal.h>
#include <os/include/sys/__assert.h>

FUNC_NORETURN __weak void arch_system_halt(unsigned int reason)
{
	ARG_UNUSED(reason);

	/* TODO: What's the best way to totally halt the system if SMP
	 * is enabled?
	 */

	(void)arch_irq_lock();
	for (;;) {
		/* Spin endlessly */
	}
}

// TODO

__weak void k_sys_fatal_error_handler(unsigned int reason,
				      const z_arch_esf_t *esf)
{
	ARG_UNUSED(esf);
	//LOG_PANIC();
	//LOG_ERR("Halting system");
	arch_system_halt(reason);
	CODE_UNREACHABLE;
}

void z_fatal_error(unsigned int reason, const z_arch_esf_t *esf)
{

    /* We can't allow this code to be preempted, but don't need to
	 * synchronize between CPUs, so an arch-layer lock is
	 * appropriate.
	 */
	unsigned int key = arch_irq_lock();

    // TODO

    k_sys_fatal_error_handler(reason, esf);

	/* If the system fatal error handler returns, then kill the faulting
	 * thread; a policy decision was made not to hang the system.
	 *
	 * Policy for fatal errors in ISRs: unconditionally panic.
	 *
	 * There is one exception to this policy: a stack sentinel
	 * check may be performed (on behalf of the current thread)
	 * during ISR exit, but in this case the thread should be
	 * aborted.
	 *
	 * Note that k_thread_abort() returns on some architectures but
	 * not others; e.g. on ARC, x86_64, Xtensa with ASM2, ARM
	 */

    arch_irq_unlock(key);
}