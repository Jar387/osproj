#ifndef TTY_H
#define TTY_H

#define TTY_CLEAR 1
void preinit_tty();
void init_tty();

int putchar(char c);		// for pre-arch_init debug use
		     // do not abuse :)

#endif
