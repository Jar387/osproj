#include <drivers/char.h>
#include <stdarg.h>
#include <asm/ring0.h>
#include <drivers/char/tty.h>

#define IS_NUM(num) ({ \
	int result; \
	if(num>='0'&&num<='9'){ \
		result = 1; \
	}else{ \
		result = 0; \
	} \
	result; \
})

static void
putchar(char c)
{
	tty_write(MINOR_TTY_TTY1, c);
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
x(int val, int padding)
{
	if (val == 0) {
		for (int j = 0; j < padding; j++) {
			puthex(0);
		}
		return;
	}
	int i, first = 0, digcount = 0, startdig = 0, val2 = val;
	for (i = 0; i < 8; i++) {
		if (((val & 0xf0000000) >> 28) != 0 && first == 0) {
			first = 1;
			startdig = i;
		}
		if (first == 1) {
			digcount++;
		}
		val <<= 4;
	}
	if (padding > digcount) {
		padding -= digcount;
		for (; padding > 0; padding--) {
			puthex(0);
		}
	}
	for (i = 0; i < 8; i++) {
		if (i >= startdig) {
			puthex(((val2 & 0xf0000000) >> 28) & 0xF);
		}
		val2 <<= 4;
	}
}

static void
X(int val, int padding)
{
	int i, count = 0;
	int non_zero_found = 0;

	for (i = 0; i < 8; i++) {
		int digit = (val & 0xf0000000) >> 28;

		if (digit != 0 || non_zero_found) {
			putheX(digit);
			non_zero_found = 1;
			count++;
		}

		val <<= 4;
	}
	while (count < padding) {
		puthex(0);
		count++;
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
				if (IS_NUM(fmt[1])) {
					x(va_arg(varlist, unsigned int),
					  fmt[1] - '0');
					fmt++;
					goto fin;
				}
				x(va_arg(varlist, unsigned int), 0);
				goto fin;
			case 'X':
				X(va_arg(varlist, unsigned int), 0);
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
