#pragma once

#include <os/include/kernel/thread.h>
#include <os/include/kernel.h>

void idle(void *unused1, void *unused2, void *unused3);

static inline bool z_is_thread_suspended(struct k_thread *thread)
{
	return (thread->base.thread_state & _THREAD_SUSPENDED) != 0U;
}

static inline bool z_is_thread_pending(struct k_thread *thread)
{
	return (thread->base.thread_state & _THREAD_PENDING) != 0U;
}

static inline bool z_is_thread_prevented_from_running(struct k_thread *thread)
{
	uint8_t state = thread->base.thread_state;

	return (state & (_THREAD_PENDING | _THREAD_PRESTART | _THREAD_DEAD |
			 _THREAD_DUMMY | _THREAD_SUSPENDED)) != 0U;

}

static inline bool z_is_thread_state_set(struct k_thread *thread, uint32_t state)
{
	return (thread->base.thread_state & state) != 0U;
}

static inline bool z_is_thread_queued(struct k_thread *thread)
{
	return z_is_thread_state_set(thread, _THREAD_QUEUED);
}

static inline void z_mark_thread_as_suspended(struct k_thread *thread)
{
	thread->base.thread_state |= _THREAD_SUSPENDED;
}

static inline void z_mark_thread_as_not_suspended(struct k_thread *thread)
{
	thread->base.thread_state &= ~_THREAD_SUSPENDED;
}

static inline void z_mark_thread_as_started(struct k_thread *thread)
{
	thread->base.thread_state &= ~_THREAD_PRESTART;
}

static inline void z_mark_thread_as_pending(struct k_thread *thread)
{
	thread->base.thread_state |= _THREAD_PENDING;
}

static inline void z_mark_thread_as_not_pending(struct k_thread *thread)
{
	thread->base.thread_state &= ~_THREAD_PENDING;
}

static inline void z_set_thread_states(struct k_thread *thread, uint32_t states)
{
	thread->base.thread_state |= states;
}

static inline void z_reset_thread_states(struct k_thread *thread,
					uint32_t states)
{
	thread->base.thread_state &= ~states;
}

static inline bool z_is_prio_higher_or_equal(int prio1, int prio2)
{
	return prio1 <= prio2;
}

static inline bool z_is_prio_higher(int prio, int test_prio)
{
	return prio < test_prio;
}

static inline bool z_is_prio_lower_or_equal(int prio1, int prio2)
{
	return prio1 >= prio2;
}

int32_t z_sched_prio_cmp(struct k_thread *thread_1, struct k_thread *thread_2);

static inline bool z_is_idle_thread_entry(void *entry_point)
{
	return entry_point == idle;
}

static inline bool _is_valid_prio(int prio, void *entry_point)
{
	if (prio == K_IDLE_PRIO && z_is_idle_thread_entry(entry_point)) {
		return true;
	}

	if (!z_is_prio_higher_or_equal(prio,
				       K_LOWEST_APPLICATION_THREAD_PRIO)) {
		return false;
	}

	if (!z_is_prio_lower_or_equal(prio,
				      K_HIGHEST_APPLICATION_THREAD_PRIO)) {
		return false;
	}

	return true;
}


static inline bool z_is_idle_thread_object(struct k_thread *thread)
{
#ifdef CONFIG_MULTITHREADING
	return thread == &z_idle_threads[0];
#else
	return false;
#endif /* CONFIG_MULTITHREADING */
}