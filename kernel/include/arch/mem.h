#ifndef ARCH_MEM_H
#define ARCH_MEM_H

#define HIGHER_HALF_DATA	0xFFFF800000000000UL
#define HIGHER_HALF_CODE	0xFFFFFFFF80000000UL
#define GiB 0x40000000UL
#define PAGE_ADDR_MASK 0x000FFFFFFFFFF000

#define DEFAULT_PAGE_SIZE 0x1000
#define PHYS_TO_HIGHER_HALF_DATA(address)   (HIGHER_HALF_DATA + address)
#define PHYS_TO_HIGHER_HALF_CODE(address)   (HIGHER_HALF_CODE + address)

#define HIGHER_HALF_DATA_TO_PHYS(address)   (address - HIGHER_HALF_DATA)
#define HIGHER_HALF_CODE_TO_PHYS(address)   (address - HIGHER_HALF_CODE)

#define ALIGN_DOWN(address, align)  ((address) & ~((align)-1))
#define ALIGN_UP(address, align)    (((address) + (align)-1) & ~((align)-1))

#endif
