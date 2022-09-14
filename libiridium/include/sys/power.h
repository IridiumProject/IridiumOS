#ifndef LIBIRIDIUM__SYS__POWER_H
#define LIBIRIDIUM__SYS__POWER_H

#include <stdint.h>


#define REBOOT_MAG 0xCA7F00D


void reboot(uint32_t reboot_mag);


#endif
