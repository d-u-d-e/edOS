#pragma once

#include <arch/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Lock interrupts on the current CPU
 *
 * @see irq_lock()
 */
static inline unsigned int arch_irq_lock(void);

/**
 * Unlock interrupts on the current CPU
 *
 * @see irq_unlock()
 */
static inline void arch_irq_unlock(unsigned int key);

/**
 * Test if calling arch_irq_unlock() with this key would unlock irqs
 *
 * @param key value returned by arch_irq_lock()
 * @return true if interrupts were unlocked prior to the arch_irq_lock()
 * call that produced the key argument.
 */
static inline bool arch_irq_unlocked(unsigned int key);


/**
 * @brief Perform architecture specific processing within spin loops
 *
 * This is invoked from busy loops with IRQs disabled such as the contended
 * spinlock loop. The default implementation is a weak function that calls
 * arch_nop(). Architectures may implement this function to perform extra
 * checks or power management tricks if needed.
 */
void arch_spin_relax(void);

#ifdef __cplusplus
}
#endif