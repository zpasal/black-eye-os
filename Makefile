arch ?= x86_64
kernel := build/kernel-$(arch).bin
iso := build/hos-$(arch).iso
linker_script := src/linker.ld
grub_cfg := src/grub.cfg
assembly_source_files := $(wildcard src/*.asm)
assembly_object_files := $(patsubst src/%.asm, \
	build/%.o, $(assembly_source_files))
cc := gcc
c_include := 'src/include'
cflags := -fno-pic -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		  -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
	      -I $(c_include) -nostartfiles -nodefaultlibs -fno-exceptions \
	      -Wall -Wextra -Werror -c -mcmodel=large 
c_source_files := $(wildcard src/*.c) $(wildcard src/lib/*.c)
c_object_files := $(patsubst src/%.c, \
	build/%.o, $(c_source_files))

.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -r build

run: $(iso)
	@qemu-system-x86_64 -cdrom $(iso)

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

$(kernel): $(assembly_object_files) $(c_object_files) $(linker_script)
	@ld -nostdlib -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_object_files)
	@objdump -D $(kernel) > build/dump.asm
	@objdump -x $(kernel) >> build/dump.asm

# compile assembly files
build/%.o: src/%.asm
	@mkdir -p $(shell dirname $@)
	@nasm -w-number-overflow -felf64 $< -o $@

# compile c files
build/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	@gcc $(cflags) $< -o $@
