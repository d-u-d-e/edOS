#pragma once

#include <os/include/sys/dlist.h>
#include <os/include/types.h>
#include <os/include/kernel/sched_priq.h>
#include <arch/structs.h>
#include <os/include/atomic.h>

/* kernel timeout record */

struct _timeout;
typedef void (*_timeout_func_t)(struct _timeout *t);

struct _timeout {
	sys_dnode_t node;
	_timeout_func_t fn;
#ifdef CONFIG_TIMEOUT_64BIT
	int64_t dticks;
#else
	int32_t dticks;
#endif
};

typedef struct {
	sys_dlist_t waitq;
} _wait_q_t;

#ifdef CONFIG_STACK_SENTINEL
/* Magic value in lowest bytes of the stack */
#define STACK_SENTINEL 0xF0F0F0F0
#endif

/* lowest value of _thread_base.preempt at which a thread is non-preemptible */
#define _NON_PREEMPT_THRESHOLD 0x0080U

/* highest value of _thread_base.preempt at which a thread is preemptible */
#define _PREEMPT_THRESHOLD (_NON_PREEMPT_THRESHOLD - 1U)

/*
 * Bitmask definitions for the struct k_thread.thread_state field.
 *
 * Must be before kernel_arch_data.h because it might need them to be already
 * defined.
 */

/* states: common uses low bits, arch-specific use high bits */

/* Not a real thread */
#define _THREAD_DUMMY (BIT(0))

/* Thread is waiting on an object */
#define _THREAD_PENDING (BIT(1))

/* Thread has not yet started */
#define _THREAD_PRESTART (BIT(2))

/* Thread has terminated */
#define _THREAD_DEAD (BIT(3))

/* Thread is suspended */
#define _THREAD_SUSPENDED (BIT(4))

/* Thread is being aborted */
#define _THREAD_ABORTING (BIT(5))

/* Thread is present in the ready queue */
#define _THREAD_QUEUED (BIT(7))

struct _ready_q {
	/* always contains next thread to run: cannot be NULL */
	struct k_thread *cache;
#if defined(CONFIG_SCHED_DUMB)
	sys_dlist_t runq;
#elif defined(CONFIG_SCHED_MULTIQ)
	struct _priq_mq runq;
#endif
};

typedef struct _ready_q _ready_q_t;

struct _cpu {

	/* nested interrupt count */
	uint32_t nested;

	/* currently scheduled thread */
	struct k_thread *current;

	/* one assigned idle thread per CPU */
	struct k_thread *idle_thread;

	uint8_t id;

#if defined(CONFIG_FPU_SHARING)
	void *fp_ctx;
#endif

	/* Per CPU architecture specifics */
	struct _cpu_arch arch;
};

struct z_kernel {
	struct _cpu cpus[CONFIG_MP_MAX_NUM_CPUS];

#ifdef CONFIG_PM
	int32_t idle; /* Number of ticks for kernel idling */
#endif

	struct _ready_q ready_q;

#ifdef CONFIG_FPU_SHARING
	/* thread that owns the FP regs */
	struct k_thread *current_fp;
#endif

};

typedef struct z_kernel _kernel_t;

extern struct z_kernel _kernel;
extern atomic_t _cpus_active;

#define _current_cpu (&_kernel.cpus[0])
#define _current _kernel.cpus[0].current