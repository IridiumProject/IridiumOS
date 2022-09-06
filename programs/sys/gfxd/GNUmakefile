OUTPUT = gfxd.sys
CFILES = $(shell find src/ -name "*.c")
CC = x86_64-elf-gcc
LIBC_PATH = ../../../libiridium/


.PHONY: all
all: build link

build: $(CFILES)
	$(CC) $^ -Iinclude/ -I$(LIBC_PATH)/include \
	    -std=gnu11           \
	    -ffreestanding       \
	    -fno-stack-protector \
	    -fno-pic             \
        -Werror=implicit     \
        -Werror=implicit-function-declaration  \
        -Werror=implicit-int \
        -Werror=int-conversion \
        -Werror=incompatible-pointer-types \
        -Werror=int-to-pointer-cast        \
	    -mabi=sysv           \
	    -mno-80387           \
	    -mno-mmx             \
	    -mno-3dnow           \
	    -mno-sse             \
	    -mno-sse2            \
	    -mno-red-zone        \
		-c

link:
	ld *.o -Tlink.ld -L$(LIBC_PATH) -l:lib_iridium.a -o ../../bin/$(OUTPUT)
	rm *.o
