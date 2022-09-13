OUTPUT = gfxd.sys
CFILES = $(shell find src/ -name "*.c")
CC = x86_64-elf-gcc


.PHONY: all
all: buildc buildasm lib

.PHONY: buildasm
buildasm: $(shell find src/ -name "*.asm")
	@ echo -e "\n\n\n\n\nBUILDING ASM FILES\n\n\n\n"
	for i in $$(find src/ -name "*.asm"); do nasm -felf64 $$i ;  if [ $$? != 0 ]; then echo -e "\n\nERROR BUILDING ASM!!\n\n"; killall make; fi; done
	for i in $$(find src/ -name "*.o"); do mv $$i ./; done
	@ echo -e "\n\n\n\n\nASM BUILT\n\n\n\n"

.PHONY: buildc
buildc: $(CFILES)
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

.PHONY: lib
lib:
	ar rcs lib_iridium.a *.o
	rm *.o
