#include <arch/cpu.h>
#include <os/include/atomic.h>


#ifdef __cplusplus
extern "C" {
#endif

struct z_spinlock_key {
	int key;
};

struct k_spinlock {

#ifdef CONFIG_SMP
    atomic_t locked;
#endif

#if defined(CONFIG_CPP) && !defined(CONFIG_SMP)

    /* If CONFIG_SMP is not defined
	 * the k_spinlock struct will have no members. The result
	 * is that in C sizeof(k_spinlock) is 0 and in C++ it is 1.
	 *
	 * This size difference causes problems when the k_spinlock
	 * is embedded into another struct like k_msgq, because C and
	 * C++ will have different ideas on the offsets of the members
	 * that come after the k_spinlock member.
	 *
	 * To prevent this we add a 1 byte dummy member to k_spinlock
	 * when the user selects C++ support and k_spinlock would
	 * otherwise be empty.
	 */
    char dummy;
#endif

};

/*
 * This type defines a "key" value used by a spinlock implementation
 * to store the system interrupt state at the time of a call to
 * k_spin_lock().  It is expected to be passed to a matching
 * k_spin_unlock().
*/
typedef struct z_spinlock_key k_spinlock_key_t;

/**
 * @brief Lock a spinlock
 *
 * This routine locks the specified spinlock, returning a key handle
 * representing interrupt state needed at unlock time.  Upon
 * returning, the calling thread is guaranteed not to be suspended or
 * interrupted on its current CPU until it calls k_spin_unlock().  The
 * implementation guarantees mutual exclusion: exactly one thread on
 * one CPU will return from k_spin_lock() at a time.  Other CPUs
 * trying to acquire a lock already held by another CPU will enter an
 * implementation-defined busy loop ("spinning") until the lock is
 * released.
 *
 * Separate spin locks may be nested. It is legal to lock an
 * (unlocked) spin lock while holding a different lock.  Spin locks
 * are not recursive, however: an attempt to acquire a spin lock that
 * the CPU already holds will deadlock.
 *
 * In circumstances where only one CPU exists, the behavior of
 * k_spin_lock() remains as specified above, though obviously no
 * spinning will take place.  Implementations may be free to optimize
 * in uniprocessor contexts such that the locking reduces to an
 * interrupt mask operation.
 *
 * @param l A pointer to the spinlock to lock
 * @return A key value that must be passed to k_spin_unlock() when the
 *         lock is released.
 */

static ALWAYS_INLINE k_spinlock_key_t k_spin_lock(struct k_spinlock *l)
{
	ARG_UNUSED(l);
	k_spinlock_key_t k;

	/* Note that we need to use the underlying arch-specific lock
	 * implementation.  The "irq_lock()" API in SMP context is
	 * actually a wrapper for a global spinlock!
	 */
	k.key = arch_irq_lock();
#ifdef CONFIG_SMP
	while (!atomic_cas(&l->locked, 0, 1)) {
		arch_spin_relax();
	}
#endif
	return k;
}

/**
 * @brief Unlock a spin lock
 *
 * This releases a lock acquired by k_spin_lock().  After this
 * function is called, any CPU will be able to acquire the lock.  If
 * other CPUs are currently spinning inside k_spin_lock() waiting for
 * this lock, exactly one of them will return synchronously with the
 * lock held.
 *
 * Spin locks must be properly nested.  A call to k_spin_unlock() must
 * be made on the lock object most recently locked using
 * k_spin_lock(), using the key value that it returned.  Attempts to
 * unlock mis-nested locks, or to unlock locks that are not held, or
 * to passing a key parameter other than the one returned from
 * k_spin_lock(), are illegal.  When CONFIG_SPIN_VALIDATE is set, some
 * of these errors can be detected by the framework.
 *
 * @param l A pointer to the spinlock to release
 * @param key The value returned from k_spin_lock() when this lock was
 *        acquired
 */
static ALWAYS_INLINE void k_spin_unlock(struct k_spinlock *l,
					k_spinlock_key_t key)
{
	ARG_UNUSED(l);
#ifdef CONFIG_SMP
	/* Strictly we don't need atomic_clear() here (which is an
	 * exchange operation that returns the old value).  We are always
	 * setting a zero and (because we hold the lock) know the existing
	 * state won't change due to a race.  But some architectures need
	 * a memory barrier when used like this, and we don't have a
	 * Zephyr framework for that.
	 */
	atomic_clear(&l->locked);
#endif
	arch_irq_unlock(key.key);
}

/* Internal function: releases the lock, but leaves local interrupts disabled */
static ALWAYS_INLINE void k_spin_release(struct k_spinlock *l)
{
	ARG_UNUSED(l);
#ifdef CONFIG_SMP
	atomic_clear(&l->locked);
#endif
}

/**
 * @brief Leaves a code block guarded with @ref K_SPINLOCK after releasing the
 * lock.
 *
 * See @ref K_SPINLOCK for details.
 */
#define K_SPINLOCK_BREAK continue

/**
 * @brief Guards a code block with the given spinlock, automatically acquiring
 * the lock before executing the code block. The lock will be released either
 * when reaching the end of the code block or when leaving the block with
 * @ref K_SPINLOCK_BREAK.
 *
 * @details Example usage:
 *
 * @code{.c}
 * K_SPINLOCK(&mylock) {
 *
 *   ...execute statements with the lock held...
 *
 *   if (some_condition) {
 *     ...release the lock and leave the guarded section prematurely:
 *     K_SPINLOCK_BREAK;
 *   }
 *
 *   ...execute statements with the lock held...
 *
 * }
 * @endcode
 *
 * Behind the scenes this pattern expands to a for-loop whose body is executed
 * exactly once:
 *
 * @code{.c}
 * for (k_spinlock_key_t key = k_spin_lock(&mylock); ...; k_spin_unlock(&mylock, key)) {
 *     ...
 * }
 * @endcode
 *
 * @warning The code block must execute to its end or be left by calling
 * @ref K_SPINLOCK_BREAK. Otherwise, e.g. if exiting the block with a break,
 * goto or return statement, the spinlock will not be released on exit.
 *
 * @note In user mode the spinlock must be placed in memory accessible to the
 * application, see @ref K_APP_DMEM and @ref K_APP_BMEM macros for details.
 *
 * @param lck Spinlock used to guard the enclosed code block.
 */
#define K_SPINLOCK(lck)                                                                            \
	for (k_spinlock_key_t __i K_SPINLOCK_ONEXIT = {}, __key = k_spin_lock(lck); !__i.key;      \
	     k_spin_unlock(lck, __key), __i.key = 1)

/** @} */


#ifdef __cplusplus
}
#endif