#include <os/include/kernel/ksched.h>

static inline int is_preempt(struct k_thread *thread)
{
	/* explanation in kernel_struct.h */
	return thread->base.preempt <= _PREEMPT_THRESHOLD;
}