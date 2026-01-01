# Toolchain definitions
CC = riscv64-unknown-elf-gcc
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany -march=rv64ima_zicsr -mabi=lp64 -g -Wall -Iinclude --freestanding

# Source files
SRCS = src/entry.S src/kernel.c src/uart.c src/trap.c src/trap.S src/syscall.c src/lib.c src/kalloc.c src/proc.c

all: kernel.elf

kernel.elf: $(SRCS) kernel.ld
	$(CC) $(CFLAGS) -T kernel.ld -o kernel.elf $(SRCS)

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf

debug: kernel.elf
	qemu-system-riscv64 -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf -s -S

clean:
	rm -f kernel.elf
