#pragma once

#if defined(__GNUC__)
#include <arch/arm/asm_inline_gcc.h>
#else
#include <arch/arm/asm_inline_other.h>
#endif