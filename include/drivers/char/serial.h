#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_DATA(port) (port)
#define SERIAL_INT(port) (port+1)
#define SERIAL_FIFO(port) (port+2)
#define SERIAL_LINECTL(port) (port+3)
#define SERIAL_MODEM(port) (port+4)

void serial_init();

#endif
