#pragma once

#include <os/include/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_SYS_CLOCK_EXISTS

static inline void z_init_timeout(struct _timeout *to)
{
	sys_dnode_init(&to->node);
}

static inline bool z_is_inactive_timeout(const struct _timeout *to)
{
	return !sys_dnode_is_linked(&to->node);
}

#endif
#ifdef __cplusplus
}
#endif

