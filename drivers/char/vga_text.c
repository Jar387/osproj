#include <asm/cpuins.h>

#define VRAM 0xc00b8000

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

static void put_char(char c){
	int pos = y*80+x;
	pos*=2;
	char* vram = (char*)VRAM; // TODO: replace with phy2virt
	vram[pos] = c;
	step_cursor();
}

int vga_text_read(char* buf, int count){
	return -1;
}

int vga_text_write(char* buf, int count){
	for(int i=0;i<count;i++){
		put_char(buf[i]);
	}
}