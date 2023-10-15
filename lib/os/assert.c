#include <os/include/kernel.h>
#include <stdarg.h>

#ifdef CONFIG_ASSERT_NO_FILE_INFO
__weak void assert_post_action(void)
#else
__weak void assert_post_action(const char *file, unsigned int line)
#endif
{
    k_panic();
}

void assert_print(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintk(fmt, ap);
	va_end(ap);
}