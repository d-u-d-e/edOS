#pragma once

#include <config/config.h>

#include <arch/arm/asm_inline.h>
#include <arch/arm/exc.h>
#include <arch/arm/thread.h>

#if defined(CONFIG_CPU_AARCH32_CORTEX_A)
#include <arch/arm/cortex-a/cpu.h>
#else
#error "Unknown ARM Architecture"
#endif