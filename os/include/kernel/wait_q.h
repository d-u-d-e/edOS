#pragma once

#include <os/include/kernel/kernel_structs.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void z_waitq_init(_wait_q_t *w)
{
	sys_dlist_init(&w->waitq);
}

#define _WAIT_Q_FOR_EACH(wq, thread_ptr) \
	SYS_DLIST_FOR_EACH_CONTAINER(&((wq)->waitq), thread_ptr, \
				     base.qnode_dlist)

static inline struct k_thread *z_waitq_head(_wait_q_t *w)
{
	return (struct k_thread *)sys_dlist_peek_head(&w->waitq);
}


#define Z_WAIT_Q_INIT(wait_q) { SYS_DLIST_STATIC_INIT(&(wait_q)->waitq) }

#ifdef __cplusplus
}
#endif
