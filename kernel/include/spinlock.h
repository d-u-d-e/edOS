#include <arch/cpu.h>
#include <kernel/include/atomic.h>

struct k_spinlock {

#ifdef CONFIG_SMP
    atomic_t locked;
#endif
};