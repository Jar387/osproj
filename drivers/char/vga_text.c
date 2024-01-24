#include <asm/ring0.h>

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

int vga_text_readc(char* c){
	return -1;
}

int vga_text_writec(char c){
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
			vga_text_writec('\0');
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
	char* vram = (char*)VRAM; // TODO: replace with phy2virt
	vram[pos] = c;
	step_cursor();
}

int vga_text_read(char* buf, int count){
	return -1;
}

int vga_text_write(char* buf, int count){
	for(int i=0;i<count;i++){
		vga_text_writec(buf[i]);
	}
}