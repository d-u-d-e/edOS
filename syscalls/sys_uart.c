#include <stdint.h>
#include <os/include/device.h>
#include <os/include/toolchain/toolchain.h>

//TODO: in Zephyr these are auto-generated

extern int z_impl_uart_err_check(const struct device * dev);

static inline int uart_err_check(const struct device * dev)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		union { uintptr_t x; const struct device * val; } parm0 = { .val = dev };
		return (int) arch_syscall_invoke1(parm0.x, K_SYSCALL_UART_ERR_CHECK);
	}
#endif
	compiler_barrier();
	return z_impl_uart_err_check(dev);
}


extern int z_impl_uart_poll_in(const struct device * dev, unsigned char * p_char);
static inline int uart_poll_in(const struct device * dev, unsigned char * p_char)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		union { uintptr_t x; const struct device * val; } parm0 = { .val = dev };
		union { uintptr_t x; unsigned char * val; } parm1 = { .val = p_char };
		return (int) arch_syscall_invoke2(parm0.x, parm1.x, K_SYSCALL_UART_POLL_IN);
	}
#endif
	compiler_barrier();
	return z_impl_uart_poll_in(dev, p_char);
}

extern void z_impl_uart_poll_out(const struct device * dev, unsigned char out_char);

static inline void uart_poll_out(const struct device * dev, unsigned char out_char)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		union { uintptr_t x; const struct device * val; } parm0 = { .val = dev };
		union { uintptr_t x; unsigned char val; } parm1 = { .val = out_char };
		(void) arch_syscall_invoke2(parm0.x, parm1.x, K_SYSCALL_UART_POLL_OUT);
		return;
	}
#endif
	compiler_barrier();
	z_impl_uart_poll_out(dev, out_char);
}