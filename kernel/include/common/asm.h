#ifndef ASM_H
#define ASM_H

#define CLI asm volatile("cli")
#define STI asm volatile("sti")
#define HLT asm volatile("hlt")
#define CLI_SLEEP for (uint32_t i = 0; i < 99999999; ++i) {asm volatile("cli");}


#endif
