#include <os/include/kernel.h>

/* the only struct z_kernel instance */
struct z_kernel _kernel;

atomic_t _cpus_active;

#ifdef CONFIG_MULTITHREADING
struct k_thread z_idle_threads[CONFIG_MP_MAX_NUM_CPUS];
#endif /* CONFIG_MULTITHREADING */