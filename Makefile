# Toolchain definitions
CC = riscv64-unknown-elf-gcc
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany -march=rv64ima_zicsr -mabi=lp64 -g -Wall

# Source files
SRCS = entry.S kernel.c uart.c trap.c trap.S

all: kernel.elf

kernel.elf: $(SRCS) kernel.ld
	$(CC) $(CFLAGS) -T kernel.ld -o kernel.elf $(SRCS)

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf

clean:
	rm -f kernel.elf
