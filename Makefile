# Toolchain
CC      = riscv64-unknown-elf-gcc
AS      = riscv64-unknown-elf-as
LD      = riscv64-unknown-elf-ld
OBJCOPY = riscv64-unknown-elf-objcopy

CFLAGS  = -nostdlib -fno-builtin -mcmodel=medany -march=rv64ima_zicsr_zifencei -mabi=lp64 -g -Wall -Iinclude --freestanding
LDFLAGS = -T kernel.ld

# Kernel sources
KSRCS = src/entry.S src/kernel.c src/uart.c src/trap.c src/trap.S src/syscall.c src/lib.c src/kalloc.c src/proc.c src/vm.c

# User init program
USER_INIT_SRC = user/init.S
USER_INIT_ELF = user/init.elf
USER_INIT_BIN = user/init.bin
USER_INIT_OBJ = user/initbin.o

all: kernel.elf

# --- Build the user program as a tiny standalone image ---

# Assemble/link user init at VA 0x0 (simple for early bringup)
# Note: no libc; you write pure asm (or use gcc with -nostdlib).
$(USER_INIT_ELF): $(USER_INIT_SRC)
	$(CC) -nostdlib -ffreestanding -march=rv64ima_zicsr -mabi=lp64 -Wl,-Ttext=0 -o $@ $<

# Convert user init ELF -> flat binary
$(USER_INIT_BIN): $(USER_INIT_ELF)
	$(OBJCOPY) -O binary $< $@

# Wrap flat binary as a linkable .o so it can be included in kernel.elf
$(USER_INIT_OBJ): $(USER_INIT_BIN)
	$(LD) -r -b binary $< -o $@

# --- Build kernel, linking the embedded user blob object in ---
kernel.elf: $(KSRCS) $(USER_INIT_OBJ) kernel.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(KSRCS) $(USER_INIT_OBJ)

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf

debug: kernel.elf
	qemu-system-riscv64 -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf -s -S

clean:
	rm -f kernel.elf
	rm -f $(USER_INIT_ELF) $(USER_INIT_BIN) $(USER_INIT_OBJ)
