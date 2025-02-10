#ifndef DEVMEM_H
#define DEVMEM_H

#define MINOR_DEVMEM_MEM 1
#define MINOR_DEVMEM_NULL 2
#define MINOR_DEVMEM_PORT 3
#define MINOR_DEVMEM_FULL 4
#define MINOR_DEVMEM_ZERO 5
#define MINOR_DEVMEM_RANDOM 6

#define MEM_DEV_HI (char*)0xc0100000
#define MEM_DEV_LO (char*)0xc0000000

#define PORT_DEV_HI 0xffff
#define PORT_DEV_LO 0

void devmem_init();

#endif
