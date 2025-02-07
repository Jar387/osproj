#include <asm/ring0.h>
#include <multiboot.h>
#include <drivers/char.h>
#include <drivers/char/tty.h>
#include <printk.h>
#include <panic.h>
#include <stddef.h>
#include <kernel.h>
#include <mm/mmap.h>

static int VRAM;

static int x = 0;
static int y = 0;

static int
move_cursor(unsigned int x, unsigned int y)
{
	if (x >= 80 || y >= 25) {
		return -1;	// error
	}
	unsigned short pos = y * 80 + x;
	outb(0x3d4, 0x0f);
	outb(0x3d5, (unsigned char) (pos & 0xff));
	outb(0x3d4, 0x0e);
	outb(0x3d5, (unsigned char) ((pos >> 8) & 0xff));
}

static void
step_cursor()
{
	if (x == 79) {
		x = 0;
		if (y != 24) {
			y++;
		}
	} else {
		x++;
	}
	move_cursor(x, y);
}

static void
scrup()
{
	char *dst = (char *) (VRAM);
	char *src = (char *) (VRAM + 2 * 80);
	char c;
	while (src != (char *) (VRAM + 2 * 80 * 25)) {
		c = *src;
		*dst = c;
		src++;
		dst++;
	}
	for (int i = 0; i < 80; i++) {
		*dst = '\0';
		dst += 2;
	}
}

static void
linebreak()
{
	x = 0;
	if (y == 24) {
		scrup();
	} else {
		y++;

	}
	move_cursor(x, y);
}

static void
cls()
{
	char *dst = (char *) (VRAM);
	for (int i = 0; i < 80 * 25; i++) {
		*dst = '\0';
		dst += 2;
	}
	x = 0;
	y = 0;
	outb_p(0x3D4, 0x0A);
	outb_p(0x3D5, (inb(0x3D5) & 0xC0) | 14);

	outb_p(0x3D4, 0x0B);
	outb_p(0x3D5, (inb(0x3D5) & 0xE0) | 15);
	move_cursor(0, 0);
}

int
putchar(char c)
{
	switch (c) {
	case '\a':
		// TODO: PC speaker
		return 0;
	case '\b':
		if (x == 0) {
			if (y == 0) {
				return 0;
			}
			y--;
			x = 79;
			move_cursor(x, y);
			return 0;
		}
		x--;
		move_cursor(x, y);
		return 0;
	case '\f':
		cls();
		x = 0;
		y = 0;
		move_cursor(x, y);
		return 0;
	case '\n':
		linebreak();
		return 0;
	case '\r':
		x = 0;
		move_cursor(x, y);
		return 0;
	case '\t':
		for (int i = 0; i < 4; i++) {
			putchar('\0');
		}
		return 0;
	case '\v':
		if (y == 24) {
			scrup();
			return 0;
		}
		y++;
		move_cursor(x, y);
		return 0;
	}
	if (y == 24 && x == 79) {
		// need scrup
		x = 0;
		scrup();
	}
	int pos = y * 80 + x;
	pos *= 2;
	char *vram = (char *) VRAM;
	vram[pos] = c;
	step_cursor();
}

int
tty_write(short minor, char data)
{
	putchar(data);
}

static int
tty_ioctl(short minor, long cmd)
{
	switch (cmd) {
	case TTY_CLEAR:
		cls();
	default:
		return -1;
	}
}

void
preinit_tty()
{
	struct multiboot_info *info = multiboot_config;
	VRAM = info->framebuffer_addr + 0xc0000000;
	if (info->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT) {
		panic("unsupported graphic mode");
	}
	cls();
}

void
init_tty()
{
	creat_cdev(MAJOR_TTY, NULL, tty_write, tty_ioctl);
}
