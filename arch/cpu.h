#pragma once

#include <config.h>
#include <kernel/include/toolchain/toolchain.h>
#include <kernel/include/sys/arch_interface.h>

#if defined(CONFIG_ARM)
#include <arch/arm/arch.h>
#else
#error "Unknown Architecture"
#endif