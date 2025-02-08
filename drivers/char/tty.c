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
#include <mm/buddy.h>

static char *ttys[255] = { 0 };
static int xs[255] = { 0 };
static int ys[255] = { 0 };
static int fores[255] = { 0 };
static int backs[255] = { 0 };

static unsigned char curr_tty = 1;

static int VRAM;

static int x = 0;
static int y = 0;

static char fore;
static char back;

static int command_sequence_status = 0;

#define CMD_BUF_SZ 32
static char command_buf[CMD_BUF_SZ] = { 0 };

static int buf_ptr = 0;

static char jmptable[16] =
    { VGA_BLACK, VGA_RED, VGA_GREEN, VGA_BROWN, VGA_BLUE, VGA_PURPLE, VGA_CYAN,
	VGA_GRAY, VGA_DARK_GRAY, VGA_LIGHT_RED, VGA_LIGHT_GREEN, VGA_YELLOW,
	VGA_LIGHT_BLUE,
	VGA_LIGHT_PURPLE, VGA_LIGHT_CYAN, VGA_WHITE
};

static inline int
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

static inline int
atoi_comma(char *dec, int *second)
{
	for (int i = 0; i < CMD_BUF_SZ; i++) {
		if (dec[i] == ';') {
			int result = atoi(dec, ';');
			dec += (i + 1);
			*second = atoi(dec, 0);
			return result;
		}
	}
	*second = -1;
	return atoi(dec, 0);
}

static inline int
is_number_or_comma(char c)
{
	return (((c >= '0') && (c <= '9')) || (c == ';'));
}

static int
move_cursor(unsigned int x, unsigned int y)
{
	if (x >= (VGA_COLUMN + 1) || y >= (VGA_ROW + 1)) {
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
	while (src != (char *) (VRAM + 2 * VGA_COLUMN * (VGA_ROW + 1))) {
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
	for (int i = 0; i < VGA_COLUMN * (VGA_ROW + 1); i++) {
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

static int
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
	int pos = y * VGA_COLUMN + x;
	pos *= 2;
	char *vram = (char *) VRAM;
	bp();
	vram[pos] = c;
	vram[pos + 1] = ASSEMB_COLOUR;
	if (y == VGA_ROW && x == 79) {
		// need scrup
		x = 0;
		scrup();
		move_cursor(x, y);
	} else {
		step_cursor();
	}
}

static inline void
clear_from_cursor_2_end()
{
	char *vram = (char *) VRAM;
	memset(vram + ((y * VGA_COLUMN + x) << 1), 0,
	       (VGA_COLUMN * (VGA_ROW + 1) - (y * VGA_COLUMN + x)) << 1);
}

static inline void
clear_from_beginning_2_cursor()
{
	char *vram = (char *) VRAM;
	memset(vram, 0, (y * VGA_COLUMN + x) << 1);
}

static inline void
clear_line_from_cursor_2_end()
{
	char *vram = (char *) VRAM;
	memset(vram + ((y * VGA_COLUMN + x) << 1), 0, (VGA_COLUMN - x) << 1);
}

static inline void
clear_line_from_beginning_2_cursor()
{
	char *vram = (char *) VRAM;
	memset(vram + ((y * VGA_COLUMN) << 1), 0, x << 1);
}

static inline void
clear_line()
{
	char *vram = (char *) VRAM;
	memset(vram + ((y * VGA_COLUMN) << 1), 0, VGA_COLUMN << 1);
}

int
tty_write(short minor, char data)
{
	if (minor == 0) {
		// switch tty
		if (data == 0 || data == curr_tty) {
			return 0;
		}
		memcpy(ttys[curr_tty], (char *) VRAM,
		       2 * VGA_COLUMN * (VGA_ROW + 1));
		xs[curr_tty] = x;
		ys[curr_tty] = y;
		fores[curr_tty] = fore;
		backs[curr_tty] = back;
		if (ttys[data] == NULL) {
			ttys[data] = (char *) palloc(ZONE_KERNEL, 1);
			memset(ttys[data], 0, 2 * VGA_COLUMN * (VGA_ROW + 1));
		}
		curr_tty = data;
		x = xs[curr_tty];
		y = ys[curr_tty];
		fore = fores[curr_tty];
		back = backs[curr_tty];
		memcpy((char *) VRAM, ttys[curr_tty],
		       2 * VGA_COLUMN * (VGA_ROW + 1));
		return 0;
	}
	if (data == '\033') {
		command_sequence_status = 1;
		return 0;
	}
	if (data == '[' && command_sequence_status == 1) {
		command_sequence_status = 2;
		return 0;
	}
	if (command_sequence_status == 2) {
		if (is_number_or_comma(data)) {
			command_buf[buf_ptr] = data;
			buf_ptr++;
			return 0;
			if (buf_ptr >= CMD_BUF_SZ - 1) {
				goto terminate_sequence;
			}
		} else {
			int second;
			int num = atoi_comma(command_buf, &second);
			switch (data) {
			case 'A':
				if (y < num) {
					y = 0;
				} else {
					y -= num;
				}
				goto terminate_sequence;
			case 'B':
				y += num;
				if (y > VGA_ROW) {
					y = VGA_ROW;
				}
				goto terminate_sequence;
			case 'C':
				x += num;
				if (x > VGA_COLUMN) {
					x = VGA_COLUMN;
				}
				goto terminate_sequence;
			case 'D':
				if (x < num) {
					x = 0;
				} else {
					x -= num;
				}
				goto terminate_sequence;
			case 'E':
				x = 0;
				y += num;
				if (y > VGA_ROW) {
					y = VGA_ROW;
				}
				goto terminate_sequence;
			case 'F':
				x = 0;
				if (y < num) {
					y = 0;
				} else {
					y -= num;
				}
				goto terminate_sequence;
			case 'G':
				x = num;
				if (x < 0) {
					x = 0;
				}
				if (x > VGA_COLUMN) {
					x = VGA_COLUMN;
				}
				goto terminate_sequence;
			case 'H':
				x = 0;
				y = 0;
			case 'f':
				if (second != -1) {
					x = num;
					y = second;
					if (x < 0) {
						x = 0;
					}
					if (x > VGA_COLUMN) {
						x = VGA_COLUMN;
					}
					if (y < 0) {
						y = 0;
					}
					if (y > VGA_ROW) {
						y = VGA_ROW;
					}
				}
				goto terminate_sequence;
			case 'J':
				if (num == 0) {
					clear_from_cursor_2_end();
				}
				if (num == 1) {
					clear_from_beginning_2_cursor();
				}
				if (num == 2) {
					cls();
				}
				goto terminate_sequence;
			case 'K':
				if (num == 0) {
					clear_line_from_cursor_2_end();
				}
				if (num == 1) {
					clear_line_from_beginning_2_cursor();
				}
				if (num == 2) {
					clear_line();
				}
				goto terminate_sequence;
			case 'S':
			case 'T':
				cls();
				goto terminate_sequence;
			case 'm':
				if (num == 0) {
					fore = DEFAULT_FORE;
					back = DEFAULT_BACK;
				}
				if (num >= 30 && num <= 37) {
					fore = jmptable[num - 30];
				}
				if (num == 1 && second >= 30 && second <= 37) {
					fore = jmptable[second - 30 + 8];
				}
				if (num >= 40 && num <= 47) {
					back = jmptable[num - 40];
				}
				if (num == 1 && second >= 40 && second <= 47) {
					back = jmptable[second - 40 + 8];
				}
				goto terminate_sequence;
			default:
				goto terminate_sequence;
			}
		}
	}
	putchar(data);
      terminate_sequence:
	command_sequence_status = 0;
	buf_ptr = 0;
	memset(command_buf, 0, CMD_BUF_SZ);
}

void
preinit_tty()
{
	struct multiboot_info *info = multiboot_config;
	VRAM = info->framebuffer_addr + 0xc0000000;
	fore = DEFAULT_FORE;
	back = DEFAULT_BACK;
	if (info->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT) {
		panic("unsupported graphic mode");
	}
	cls();
}

void
init_tty()
{
	creat_cdev(MAJOR_TTY, NULL, tty_write, NULL);
	curr_tty = 1;
	ttys[1] = (char *) palloc(ZONE_KERNEL, 1);
	memset(fores, DEFAULT_FORE, sizeof (fores));
	memset(backs, DEFAULT_BACK, sizeof (backs));
}
