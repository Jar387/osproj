#ifndef TIME_H
#define TIME_H

// pit chip
#define PIT_DATA0 0x40
#define PIT_DATA1 0x41
#define PIT_DATA2 0x42
#define PIT_CMD 0x43

#define PIT_CHNN0 0<<6
#define PIT_CHNN1 1<<6
#define PIT_CHNN2 2<<6

#define PIT_RW_LB 1<<4
#define PIT_RW_HB 2<<4
#define PIT_RW_HL 3<<4

#define PIT_MODE_TERM 0<<1
#define PIT_MODE_RETRIGG 1<<1
#define PIT_MODE_RATEGEN 2<<1
#define PIT_MODE_SQWVGEN 3<<1
#define PIT_MODE_STRIGGS 4<<1
#define PIT_MODE_HTRIGGS 5<<1

#define PIT_BIN 0
#define PIT_BCD 1

#define HZ 100
#define DIVISOR 1193180/HZ

void pit_int();
void pit_init();

#endif