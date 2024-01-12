static inline void outb(unsigned short port, unsigned char data){
	__asm__ volatile ("outb %%al, %%dx"::"a"(data), "d"(port));
}

static inline unsigned char inb(unsigned short port){
	unsigned char result;
	__asm__ volatile ("inb %%dx":"=a"(result): "d"(port));
}

static inline void hlt(){
	__asm__ volatile ("hlt");
}

static inline void cli(){
	__asm__ volatile ("cli");
}

static inline void sti(){
	__asm__ volatile ("sti");
}