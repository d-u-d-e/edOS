#include <stdarg.h>
#include <stdint.h>

typedef int (*cbprintf_cb)(int c, void *ctx);

/* @note The functionality of this function is significantly reduced when
 * @kconfig{CONFIG_CBPRINTF_NANO} is selected.
 *
 * @param out the function used to emit each generated character.
 *
 * @param ctx context provided when invoking out
 *
 * @param format a standard ISO C format string with characters and conversion
 * specifications.
 *
 * @param ap a reference to the values to be converted.
 *
 * @param flags flags on how to process the inputs.
 *              @see Z_CBVPRINTF_PROCESS_FLAGS.
 *
 * @return the number of characters generated, or a negative error value
 * returned from invoking @p out.
 */
int z_cbvprintf_impl(cbprintf_cb out, void *ctx, const char *format,
		     va_list ap, uint32_t flags);

static inline
int cbvprintf(cbprintf_cb out, void *ctx, const char *format, va_list ap)
{
	return z_cbvprintf_impl(out, ctx, format, ap, 0);
}