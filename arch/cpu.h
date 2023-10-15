#pragma once

#include <config.h>
#include <os/include/toolchain/toolchain.h>
#include <os/include/sys/arch_interface.h>

#if defined(CONFIG_ARM)
#include <arch/arm/arch.h>
#else
#error "Unknown Architecture"
#endif