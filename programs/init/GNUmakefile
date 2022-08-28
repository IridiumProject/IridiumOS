OUTPUT = init.sys
CFILES = $(shell find src/ -name "*.c")
CC = x86_64-elf-gcc


.PHONY: all
all: build link

build: $(CFILES)
	$(CC) $^ -Iinclude/ \
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
	x86_64-elf-ld -Tlink.ld *.o -o ../bin/$(OUTPUT)
	rm *.o
