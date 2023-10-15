#include <os/include/toolchain/gcc.h>
#include <os/include/kernel.h>
#include <os/include/sys/cbprintf.h>

#if defined(CONFIG_PRINTK_SYNC)
static struct k_spinlock lock;
#endif

#ifdef CONFIG_PRINTK

/**
 * @brief Default character output routine that does nothing
 * @param c Character to swallow
 *
 * Note this is defined as a weak symbol, allowing architecture code
 * to override it where possible to enable very early logging.
 *
 * @return 0
 */
__weak int arch_printk_char_out(int c)
{
	ARG_UNUSED(c);
	/* do nothing */
	return 0;
}

int (*_char_out)(int c) = arch_printk_char_out;

static int char_out(int c, void *ctx_p)
{
	(void) ctx_p;
	return _char_out(c);
}

/**
 * @brief Install the character output routine for printk
 *
 * To be called by the platform's console driver at init time. Installs a
 * routine that outputs one ASCII character at a time.
 * @param fn putc routine to install
 */
void __printk_hook_install(int (*fn)(int c))
{
	_char_out = fn;
}

/**
 * @brief Get the current character output routine for printk
 *
 * To be called by any console driver that would like to save
 * current hook - if any - for later re-installation.
 *
 * @return a function pointer or NULL if no hook is set
 */
void *__printk_get_hook(void)
{
	return _char_out;
}

void vprintk(const char *fmt, va_list ap)
{

#ifdef CONFIG_PRINTK_SYNC
		k_spinlock_key_t key = k_spin_lock(&lock);
#endif

	    cbvprintf(char_out, NULL, fmt, ap);

#ifdef CONFIG_PRINTK_SYNC
		k_spin_unlock(&lock, key);
#endif

}

#endif