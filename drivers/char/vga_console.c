#include <asm/ring0.h>
#include <multiboot.h>
#include <drivers/char.h>
#include <drivers/char/vga_console.h>
#include <printk.h>
#include <stddef.h>

static int VRAM;

static int x = 0;
static int y = 0;

static int move_cursor(unsigned int x, unsigned int y){
	if(x>=80||y>=25){
		return -1; // error
	}
	unsigned short pos = y * 80 + x;
	outb(0x3d4, 0x0f);
	outb(0x3d5, (unsigned char)(pos&0xff));
	outb(0x3d4, 0x0e);
	outb(0x3d5, (unsigned char)((pos>>8)&0xff));
}

static void step_cursor(){
	if(x==79){
		x=0;
		if(y!=24){
			y++;
		}
	}else{
		x++;
	}
	move_cursor(x, y);
}

static void scrup(){
	char* dst = (char*)(VRAM);
	char* src = (char*)(VRAM+2*80);
	char c;
	while(src!=(char*)(VRAM+2*80*25)){
		c=*src;
		*dst=c;
		src++;
		dst++;
	}
	for(int i=0;i<80;i++){
		*dst = '\0';
		dst+=2;
	}
}

static void linebreak(){
	x = 0;
	if(y==24){
		scrup();
	}else{
		y++;

	}
	move_cursor(x, y);
}

static void cls(){
	char* dst = (char*)(VRAM);
	for(int i=0;i<80*25;i++){
		*dst = '\0';
		dst+=2;
	}
}

static int putchar(char c){
	switch(c){
	case '\a':
		// TODO: PC speaker
		return 0;
	case '\b':
		if(x==0){
			if(y==0){
				return 0;
			}
			y--;
			x=79;
			move_cursor(x,y);
			return 0;
		}
		x--;
		move_cursor(x,y);
		return 0;
	case '\f':
		cls();
		x=0;
		y=0;
		move_cursor(x,y);
		return 0;
	case '\n':
		linebreak();
		return 0;
	case '\r':
		x=0;
		move_cursor(x,y);
		return 0;
	case '\t':
		for(int i=0;i<4;i++){
			putchar('\0');
		}
		return 0;
	case '\v':
		if(y==24){
			scrup();
			return 0;
		}
		y++;
		move_cursor(x,y);
		return 0;
	}
	if(y==24&&x==79){
		// need scrup
		x=0;
		scrup();
	}
	int pos = y*80+x;
	pos*=2;
	char* vram = (char*)VRAM;
	vram[pos] = c;
	step_cursor();
}

int vga_console_write(char* buf, int count){
	for(int i=0;i<count;i++){
		putchar(buf[i]);
	}
}

static int vga_console_ioctl(long cmd){
	switch(cmd){
		case VGA_CONSOLE_IOCMD_CLS:
			cls();
		default:
			return -1;
	}
}

void vga_console_init(struct multiboot_info* info, cdev_t* stdout){
	VRAM = info->framebuffer_addr+0xc0000000;
	stdout->read = NULL;
	stdout->write = &vga_console_write;
	stdout->ioctl = &vga_console_ioctl;
	if(info->framebuffer_type!=MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT){
		printk("VGA card is in graphic mode! will use serial console at COM1\n");
	}
}