#include <drivers/char.h>
#include <stdarg.h>
#include <asm/ring0.h>

static inline void
putchar(char c)
{
	cwrite(CDEV_STDOUT, &c, 1);
}

static inline void
puthex(unsigned char hex)
{
	if (hex > 0xf) {
		return;
	}
	if (hex < 0xa) {
		putchar(hex + '0');
	} else {
		putchar(hex - 0xa + 'a');
	}
}

static inline void
putheX(unsigned char hex)
{
	if (hex > 0xf) {
		return;
	}
	if (hex < 0xa) {
		putchar(hex + '0');
	} else {
		putchar(hex - 0xa + 'A');
	}
}

static void
x(int val)
{
	for (int i = 0; i < 8; i++) {
		if (((val & 0xf0000000) >> 28) != 0) {
			puthex((val & 0xf0000000) >> 28);
		}
		val <<= 4;
	}
}

static void
X(int val)
{
	for (int i = 0; i < 8; i++) {
		if (((val & 0xf0000000) >> 28) != 0) {
			putheX((val & 0xf0000000) >> 28);
		}
		val <<= 4;
	}
}

static void
p(int val)
{
	for (int i = 0; i < 8; i++) {
		puthex((val & 0xf0000000) >> 28);
		val <<= 4;
	}
}

static void
d(int val)
{
	if (val == 0) {
		putchar('0');
		return;
	}
	if (val < 0) {
		putchar('-');
		val = -val;
	}
	int power = 10;
	int i = 0;
	char buf[10];
	while (val != 0) {
		char c = (char) (val % 10) + '0';
		buf[i] = c;
		val /= 10;
		i++;
	}
	i--;
	for (; i >= 0; i--) {
		putchar(buf[i]);
	}
}

static void
u(unsigned int val)
{
	if (val == 0) {
		putchar('0');
		return;
	}
	int power = 10;
	int i = 0;
	char buf[10];
	while (val != 0) {
		char c = (char) (val % 10) + '0';
		buf[i] = c;
		val /= 10;
		i++;
	}
	i--;
	for (; i >= 0; i--) {
		putchar(buf[i]);
	}
}

void
printk(const char *fmt, ...)
{
	va_list varlist;
	va_start(varlist, fmt);

	char c = *fmt;
	while (c != '\0') {
		if (c == '%') {
			// start fmt
			fmt++;
			c = *fmt;
			switch (c) {
			case 'd':
			case 'i':
				d(va_arg(varlist, int));
				goto fin;
			case 'u':
				u(va_arg(varlist, unsigned int));
				goto fin;
			case 's':
				printk(va_arg(varlist, char *));
				goto fin;
			case 'x':
				x(va_arg(varlist, unsigned int));
				goto fin;
			case 'X':
				X(va_arg(varlist, unsigned int));
				goto fin;
			case 'c':
				putchar(va_arg(varlist, int));
				goto fin;
			case 'p':
				p(va_arg(varlist, unsigned int));
				goto fin;
			case '%':
				putchar('%');
				goto fin;
			}
		}
		putchar(c);
	      fin:
		fmt++;
		c = *fmt;
	}
}
