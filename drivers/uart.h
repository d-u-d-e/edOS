#pragma once

#include <stddef.h>
#include <kernel/include/toolchain/toolchain.h>
#include <kernel/include/device.h>
#include <lib/libc/minimal/include/errno.h>


__subsystem struct uart_driver_api {

	/** Console I/O function */
	int (*poll_in)(const struct device *dev, unsigned char *p_char);
	void (*poll_out)(const struct device *dev, unsigned char out_char);
	int (*err_check)(const struct device *dev);
};

/**
 * @brief Check whether an error was detected.
 *
 * @param dev UART device instance.
 *
 * @retval 0 If no error was detected.
 * @retval err Error flags as defined in @ref uart_rx_stop_reason
 * @retval -ENOSYS If not implemented.
 */
__syscall int uart_err_check(const struct device *dev);

static inline int z_impl_uart_err_check(const struct device *dev)
{
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->err_check == NULL) {
		return -ENOSYS;
	}

	return api->err_check(dev);
}

/**
 * @brief Read a character from the device for input.
 *
 * This routine checks if the receiver has valid data.  When the
 * receiver has valid data, it reads a character from the device,
 * stores to the location pointed to by p_char, and returns 0 to the
 * calling thread. It returns -1, otherwise. This function is a
 * non-blocking call.
 *
 * @param dev UART device instance.
 * @param p_char Pointer to character.
 *
 * @retval 0 If a character arrived.
 * @retval -1 If no character was available to read (i.e. the UART
 *            input buffer was empty).
 * @retval -ENOSYS If the operation is not implemented.
 * @retval -EBUSY If async reception was enabled using @ref uart_rx_enable
 */
__syscall int uart_poll_in(const struct device *dev, unsigned char *p_char);

static inline int z_impl_uart_poll_in(const struct device *dev,
				      unsigned char *p_char)
{
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	if (api->poll_in == NULL) {
		return -ENOSYS;
	}
	return api->poll_in(dev, p_char);
}

/**
 * @brief Write a character to the device for output.
 *
 * This routine checks if the transmitter is full.  When the
 * transmitter is not full, it writes a character to the data
 * register. It waits and blocks the calling thread, otherwise. This
 * function is a blocking call.
 *
 * To send a character when hardware flow control is enabled, the handshake
 * signal CTS must be asserted.
 *
 * @param dev UART device instance.
 * @param out_char Character to send.
 */
__syscall void uart_poll_out(const struct device *dev,
			     unsigned char out_char);

static inline void z_impl_uart_poll_out(const struct device *dev,
					unsigned char out_char)
{
	const struct uart_driver_api *api =
		(const struct uart_driver_api *)dev->api;

	api->poll_out(dev, out_char);
}