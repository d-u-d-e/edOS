#pragma once

#include <os/include/sys/dlist.h>

/* Traditional/textbook "multi-queue" structure.  Separate lists for a
 * small number (max 32 here) of fixed priorities.  This corresponds
 * to the original Zephyr scheduler.  RAM requirements are
 * comparatively high, but performance is very fast.  Won't work with
 * features like deadline scheduling which need large priority spaces
 * to represent their requirements.
 */
struct _priq_mq {
	sys_dlist_t queues[32];
	unsigned int bitmask; /* bit 1<<i set if queues[i] is non-empty */
};

struct k_thread *z_priq_mq_best(struct _priq_mq *pq);