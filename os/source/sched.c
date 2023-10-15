#include <os/include/kernel/ksched.h>
#include <os/include/kernel/timeout_q.h>
#include <os/include/sys/__assert.h>

#if defined(CONFIG_SCHED_DUMB)
#define _priq_run_add		z_priq_dumb_add
#define _priq_run_remove	z_priq_dumb_remove
define  _priq_run_best	    z_priq_dumb_best
#elif defined(CONFIG_SCHED_MULTIQ)
#define _priq_run_add		z_priq_mq_add
#define _priq_run_remove	z_priq_mq_remove
#define _priq_run_best		z_priq_mq_best
static ALWAYS_INLINE void z_priq_mq_add(struct _priq_mq *pq,
					struct k_thread *thread);
static ALWAYS_INLINE void z_priq_mq_remove(struct _priq_mq *pq,
					   struct k_thread *thread);
#endif

static inline int is_preempt(struct k_thread *thread)
{
	/* explanation in kernel_struct.h */
	return thread->base.preempt <= _PREEMPT_THRESHOLD;
}

#if CONFIG_ASSERT
static inline bool is_thread_dummy(struct k_thread *thread)
{
	return (thread->base.thread_state & _THREAD_DUMMY) != 0U;
}
#endif

/*
 * Return value same as e.g. memcmp
 * > 0 -> thread 1 priority  > thread 2 priority
 * = 0 -> thread 1 priority == thread 2 priority
 * < 0 -> thread 1 priority  < thread 2 priority
 * Do not rely on the actual value returned aside from the above.
 * (Again, like memcmp.)
 */
int32_t z_sched_prio_cmp(struct k_thread *thread_1,
	struct k_thread *thread_2)
{
	/* `prio` is <32b, so the below cannot overflow. */
	int32_t b1 = thread_1->base.prio;
	int32_t b2 = thread_2->base.prio;

	if (b1 != b2) {
		return b2 - b1;
	}
	return 0;
}

static ALWAYS_INLINE bool should_preempt(struct k_thread *thread,
					 int preempt_ok)
{
	/* Preemption is OK if it's being explicitly allowed by
	 * software state (e.g. the thread called k_yield())
	 */
	if (preempt_ok != 0) {
		return true;
	}

	__ASSERT(_current != NULL, "");

	/* Or if we're pended/suspended/dummy (duh) */
	if (z_is_thread_prevented_from_running(_current)) {
		return true;
	}

    // TODO edge case non atomic SWAP?

	/* Otherwise we have to be running a preemptible thread or
	 * switching to a metairq
	 */
	if (is_preempt(_current)) {
		return true;
	}
	return false;
}

static ALWAYS_INLINE void *thread_runq(struct k_thread *thread)
{
	ARG_UNUSED(thread);
	return &_kernel.ready_q.runq;
}

static ALWAYS_INLINE void *curr_cpu_runq(void)
{
	return &_kernel.ready_q.runq;
}

static ALWAYS_INLINE void runq_add(struct k_thread *thread)
{
	_priq_run_add(thread_runq(thread), thread);
}

static ALWAYS_INLINE void runq_remove(struct k_thread *thread)
{
	_priq_run_remove(thread_runq(thread), thread);
}

static ALWAYS_INLINE struct k_thread *runq_best(void)
{
	return _priq_run_best(curr_cpu_runq());
}

#if defined(CONFIG_SCHED_DUMB)
static ALWAYS_INLINE void z_priq_dumb_add(sys_dlist_t *pq,
					  struct k_thread *thread)
{
	struct k_thread *t;

	__ASSERT_NO_MSG(!z_is_idle_thread_object(thread));

	SYS_DLIST_FOR_EACH_CONTAINER(pq, t, base.qnode_dlist) {
		if (z_sched_prio_cmp(thread, t) > 0) {
			sys_dlist_insert(&t->base.qnode_dlist,
					 &thread->base.qnode_dlist);
			return;
		}
	}

	sys_dlist_append(pq, &thread->base.qnode_dlist);
}
void z_priq_dumb_remove(sys_dlist_t *pq, struct k_thread *thread)
{
	ARG_UNUSED(pq);

	__ASSERT_NO_MSG(!z_is_idle_thread_object(thread));

	sys_dlist_remove(&thread->base.qnode_dlist);
}

struct k_thread *z_priq_dumb_best(sys_dlist_t *pq)
{
	struct k_thread *thread = NULL;
	sys_dnode_t *n = sys_dlist_peek_head(pq);

	if (n != NULL) {
		thread = CONTAINER_OF(n, struct k_thread, base.qnode_dlist);
	}
	return thread;
}

#endif

#ifdef CONFIG_SCHED_MULTIQ

# if (K_LOWEST_THREAD_PRIO - K_HIGHEST_THREAD_PRIO) > 31
# error Too many priorities for multiqueue scheduler (max 32)
#endif

struct k_thread *z_priq_mq_best(struct _priq_mq *pq)
{
	if (!pq->bitmask) {
		return NULL;
	}

	struct k_thread *thread = NULL;
	sys_dlist_t *l = &pq->queues[__builtin_ctz(pq->bitmask)];
	sys_dnode_t *n = sys_dlist_peek_head(l);

	if (n != NULL) {
		thread = CONTAINER_OF(n, struct k_thread, base.qnode_dlist);
	}
	return thread;
}

static ALWAYS_INLINE void z_priq_mq_add(struct _priq_mq *pq,
					struct k_thread *thread)
{
	int priority_bit = thread->base.prio - K_HIGHEST_THREAD_PRIO;

	sys_dlist_append(&pq->queues[priority_bit], &thread->base.qnode_dlist);
	pq->bitmask |= BIT(priority_bit);
}

static ALWAYS_INLINE void z_priq_mq_remove(struct _priq_mq *pq,
					   struct k_thread *thread)
{
	int priority_bit = thread->base.prio - K_HIGHEST_THREAD_PRIO;

	sys_dlist_remove(&thread->base.qnode_dlist);
	if (sys_dlist_is_empty(&pq->queues[priority_bit])) {
		pq->bitmask &= ~BIT(priority_bit);
	}
}
#endif


void init_ready_q(struct _ready_q *rq)
{
#if defined(CONFIG_SCHED_MULTIQ)
	for (int i = 0; i < ARRAY_SIZE(_kernel.ready_q.runq.queues); i++) {
		sys_dlist_init(&rq->runq.queues[i]);
	}
#else
	sys_dlist_init(&rq->runq);
#endif
}

void z_sched_init(void)
{
	init_ready_q(&_kernel.ready_q);
}