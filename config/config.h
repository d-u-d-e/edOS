#pragma once

// CORTEX A8

#define CONFIG_LITTLE_ENDIAN 1
#define CONFIG_ARM 1
#define CONFIG_ARMV7_A 1
#define CONFIG_CPU_AARCH32_CORTEX_A 1

#define CONFIG_CPU_HAS_VFP          1
#define CONFIG_CPU_HAS_FPU          1
#define CONFIG_FPU 1
#define CONFIG_CPU_HAS_MMU          1
#define CONFIG_ARM_MMU 1
#define CONFIG_CPU_HAS_MPU          1
#define CONFIG_ARM_MPU 1

// MISC
#define CONFIG_ASSERT 1
#define CONFIG_ASSERT_LEVEL 1
#define CONFIG_PRINTK 1
#define CONFIG_PRINTK_SYNC 1


#define CONFIG_SYS_CLOCK_EXISTS 1
#define CONFIG_THREAD_STACK_INFO 1
#define CONFIG_THREAD_NAME 1
#define CONFIG_THREAD_MAX_NAME_LEN 10
#define CONFIG_ERRNO 1
#define CONFIG_STACK_SENTINEL 1
#define CONFIG_NUM_COOP_PRIORITIES 16
#define CONFIG_NUM_PREEMPT_PRIORITIES 15

#define CONFIG_SCHED_MULTIQ 1

#define CONFIG_FPU_SHARING 1
#define CONFIG_MP_MAX_NUM_CPUS 1
#define CONFIG_PM 1
#define CONFIG_MULTITHREADING 1