#include <drivers/char.h>
#include <stdarg.h>

#define putchar(c) cwritec(CDEV_VGA, c)

static inline void puthex(unsigned char hex){
	if(hex>0xf){
		return;
	}
	if(hex<0xa){
		putchar(hex+'0');
	}else{
		putchar(hex-0xa+'A');
	}
}

static void printhex(int val){
	for(int i=0;i<8;i++){
		puthex((val&0xf0000000)>>28);
		val<<=4;
	}
}

static void printint(int val){
	int power = 10;
	int i = 0;
	char buf[10];
	while(val!=0){
		char c = (char)(val%10)+'0';
		buf[i] = c;
		val/=10;
		i++;
	}
	i--;
	for(;i>=0;i--){
		putchar(buf[i]);
	}
}

void printk(const char* fmt, ...){
	va_list varlist;
	va_start(varlist, fmt);

	char c = *fmt;
	while(c!='\0'){
		if(c=='%'){
			// start fmt
			fmt++;
			c = *fmt;
			switch(c){
			case 'x':
				printhex(va_arg(varlist, int));
				goto fin;
			case 's':
				printk(va_arg(varlist, char*));
				goto fin;
			case 'i':
				printint(va_arg(varlist, int));
				goto fin;
			}
		}
		putchar(c);
		fin:
		fmt++;
		c = *fmt;
	}
}