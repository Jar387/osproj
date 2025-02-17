#ifndef TTY_H
#define TTY_H

void tty_preinit();
void tty_init();

int tty_write(short minor, char data);	// for pre-arch_init debug use
		     // do not abuse :)

#define VGA_BLACK 0
#define VGA_BLUE 1
#define VGA_GREEN 2
#define VGA_CYAN 3
#define VGA_RED 4
#define VGA_PURPLE 5
#define VGA_BROWN 6
#define VGA_GRAY 7
#define VGA_DARK_GRAY 8
#define VGA_LIGHT_BLUE 9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN 11
#define VGA_LIGHT_RED 12
#define VGA_LIGHT_PURPLE 13
#define VGA_YELLOW 14
#define VGA_WHITE 15

#define DEFAULT_FORE VGA_GRAY
#define DEFAULT_BACK VGA_BLACK

#define ASSEMB_COLOUR ((back<<4)+fore)

#define VGA_ROW 24
#define VGA_COLUMN 80

#define MINOR_TTY_SELECT 0
#define MINOR_TTY_TTY1 1

#endif
