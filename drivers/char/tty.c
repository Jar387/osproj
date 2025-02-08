#include <asm/ring0.h>
#include <multiboot.h>
#include <drivers/char.h>
#include <drivers/char/tty.h>
#include <printk.h>
#include <panic.h>
#include <stddef.h>
#include <kernel.h>
#include <mm/mmap.h>
#include <lib/string.h>

static int VRAM;

static int x = 0;
static int y = 0;

static char fore;
static char back;

static int command_sequence_status = 0;

static char command_buf[16] = { 0 };

static int buf_ptr = 0;

static int
atoi(char *dec, char terminator)
{
	int i = 0;
	int result = 0;
	int base = 1;
	for (; i <= 11; i++) {
		if (dec[i] == terminator) {
			break;
		}
	}
	if (i > 10) {
		return -1;
	}
	i--;
	for (int j = i; j >= 0; j--) {
		result += base * (dec[j] - '0');
		base *= 10;
	}
	return result;
}

static int
is_number(char c)
{
	return ((c >= '0') && (c <= '9'));
}

static int
move_cursor(unsigned int x, unsigned int y)
{
	if (x >= VGA_COLUMN || y >= VGA_ROW) {
		return -1;	// error
	}
	unsigned short pos = y * VGA_COLUMN + x;
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
		if (y != VGA_ROW) {
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
	char *src = (char *) (VRAM + 2 * VGA_COLUMN);
	char c;
	while (src != (char *) (VRAM + 2 * VGA_COLUMN * VGA_ROW)) {
		c = *src;
		*dst = c;
		src++;
		dst++;
	}
	for (int i = 0; i < VGA_COLUMN; i++) {
		*dst = '\0';
		*(dst + 1) = ASSEMB_COLOUR;
		dst += 2;
	}
}

static void
linebreak()
{
	x = 0;
	if (y == VGA_ROW) {
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
	for (int i = 0; i < VGA_COLUMN * VGA_ROW; i++) {
		*dst = '\0';
		*(dst + 1) = ASSEMB_COLOUR;
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
		if (y == VGA_ROW) {
			scrup();
			return 0;
		}
		y++;
		move_cursor(x, y);
		return 0;
	}
	if (y == VGA_ROW && x == 79) {
		// need scrup
		x = 0;
		scrup();
	}
	int pos = y * VGA_COLUMN + x;
	pos *= 2;
	char *vram = (char *) VRAM;
	vram[pos] = c;
	vram[pos + 1] = ASSEMB_COLOUR;
	step_cursor();
}

int
tty_write(short minor, char data)
{
	if (data == '\033') {
		command_sequence_status = 1;
		return 0;
	}
	if (data == '[' && command_sequence_status == 1) {
		command_sequence_status = 2;
		return 0;
	}
	if (command_sequence_status == 2) {
		if (!is_number(data)) {
			command_buf[buf_ptr] = data;
			buf_ptr++;
			if (buf_ptr >= 10) {
				goto terminate_sequence;
			}
		} else {
			int num = atoi(command_buf, 0);
			switch (data) {
			case 'A':
				y -= num;
				break;
			case 'B':
				y += num;
				break;
			case 'C':
				x += num;
				break;
			case 'D':
				x -= num;
				break;
			case 'E':
				x = 0;
				y += num;
				break;
			case 'F':
				x = 0;
				y -= num;
				break;
			case 'G':
				x = num;
				break;
			case 'H':
				x = 0;
				y = 0;
				break;
			case 'J':
			case 'K':
			case 'S':
			case 'T':
				cls();
				break;
			case 'm':

				break;
			default:
				goto terminate_sequence;
			}
		}
	}
	putchar(data);
      terminate_sequence:
	command_sequence_status = 0;
	buf_ptr = 0;
	memset(command_buf, 0, 16);
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
	fore = VGA_GREEN;
	back = VGA_BLUE;
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
