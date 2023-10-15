#pragma once

#include <os/include/sys/arch_interface.h>
#include <os/include/sys/dlist.h>
#include <os/include/kernel/kernel_structs.h>

struct _thread_base {

    /* this thread's entry in a ready/wait queue */
    sys_dnode_t qnode_dlist;
    	
    /* thread state */
	uint8_t thread_state;

    	/*
	 * scheduler lock count and thread priority
	 *
	 * These two fields control the preemptibility of a thread.
	 *
	 * When the scheduler is locked, sched_locked is decremented, which
	 * means that the scheduler is locked for values from 0xff to 0x01. A
	 * thread is coop if its prio is negative, thus 0x80 to 0xff when
	 * looked at the value as unsigned.
	 *
	 * By putting them end-to-end, this means that a thread is
	 * non-preemptible if the bundled value is greater than or equal to
	 * 0x0080.
	 */
	union {
		struct {
			int8_t prio;
			uint8_t sched_locked;
		};
		uint16_t preempt;
	};

#ifdef CONFIG_SYS_CLOCK_EXISTS
	/* this thread's entry in a timeout queue */
	struct _timeout timeout;
#endif
};

typedef struct _thread_base _thread_base_t;

#if defined(CONFIG_THREAD_STACK_INFO)
/* Contains the stack information of a thread */
struct _thread_stack_info {
	/* Stack start - Represents the start address of the thread-writable
	 * stack area.
	 */
	uintptr_t start;

	/* Thread writable stack buffer size. Represents the size of the actual
	 * buffer, starting from the 'start' member, that should be writable by
	 * the thread. This comprises of the thread stack area, any area reserved
	 * for local thread data storage, as well as any area left-out due to
	 * random adjustments applied to the initial thread stack pointer during
	 * thread initialization.
	 */
	size_t size;

	/* Adjustment value to the size member, removing any storage
	 * used for TLS or random stack base offsets. (start + size - delta)
	 * is the initial stack pointer for a thread. May be 0.
	 */
	size_t delta;
};

typedef struct _thread_stack_info _thread_stack_info_t;
#endif /* CONFIG_THREAD_STACK_INFO */


struct k_thread {

	struct _thread_base base;

    /** defined by the architecture, but all archs need these */
	struct _callee_saved callee_saved;

#if defined(CONFIG_THREAD_NAME)
	/** Thread name */
	char name[CONFIG_THREAD_MAX_NAME_LEN];
#endif


#if defined(CONFIG_ERRNO) && !defined(CONFIG_ERRNO_IN_TLS) && !defined(CONFIG_LIBC_ERRNO)
	/** per-thread errno variable */
	int errno_var;
#endif

#if defined(CONFIG_THREAD_STACK_INFO)
	/** Stack Info */
	struct _thread_stack_info stack_info;
#endif /* CONFIG_THREAD_STACK_INFO */


	/** arch-specifics: must always be at the end */
	struct _thread_arch arch;
};

typedef struct k_thread _thread_t;