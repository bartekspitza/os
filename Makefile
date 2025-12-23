# Toolchain definitions
CC = riscv64-unknown-elf-gcc
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany -march=rv64ima -mabi=lp64 -g -Wall

# Source files
SRCS = entry.S kernel.c

all: kernel.elf

kernel.elf: $(SRCS) kernel.ld
	$(CC) $(CFLAGS) -T kernel.ld -o kernel.elf $(SRCS)

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf

clean:
	rm -f kernel.elf