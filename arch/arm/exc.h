#pragma once

#include <config/config.h>

#if defined(CONFIG_CPU_AARCH32_CORTEX_A)
#include <arch/arm/cortex-a/exc.h>
#else
#error Unknown ARM architecture
#endif