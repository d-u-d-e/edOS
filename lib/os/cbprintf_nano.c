#include <kernel/include/sys/cbprintf.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define OUTC(_c) do { \
	out((int)(_c), ctx); \
	++count; \
} while (false)

typedef int32_t int_value_type;
typedef uint32_t uint_value_type;
#define DIGITS_BUFLEN 10

int z_cbvprintf_impl(cbprintf_cb out, void *ctx, const char *fmt,
		     va_list ap, uint32_t flags) {

    size_t count = 0;
    char buf[DIGITS_BUFLEN];

    /* we pre-increment in the loop  afterwards */
	fmt--;

start:
	while (*++fmt != '%') {
		if (*fmt == '\0') {
			return count;
		}
		OUTC(*fmt);
	}

    for (fmt++ ; ; fmt++) {
        switch (*fmt) {
		case 0:
			return count;
		case '%':
			OUTC('%');
			goto start;
        }
    }
}