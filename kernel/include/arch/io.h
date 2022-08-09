#ifndef BUS_IO_H
#define BUS_IO_H

#include <stdint.h>

unsigned char inportb(unsigned short port);
void outportb(unsigned short port, unsigned char data);
void outportw(unsigned short port, unsigned short data);
unsigned short inportw(unsigned short port);
void outportl(unsigned short port, uint32_t data);
uint32_t inportl(unsigned short port);
void io_wait();

#endif
