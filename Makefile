CROSS = riscv64-unknown-elf-

CFLAGS = -march=rv64gc -mabi=lp64 \
         -mcmodel=medany \
         -msmall-data-limit=0 \
         -ffreestanding \
         -nostdlib -nostartfiles \
         -fno-stack-protector \
         -Wall -Iinclude

OBJS = start.o \
       trap_entry.o \
       context.o \
       block.o \
       fs.o \
       inode.o \
       treefs.o \
       main.o \
       memory.o \
       scheduler.o \
       string.o \
       task.o \
       timestamp.o \
       uart.o

all:
	$(CROSS)gcc $(CFLAGS) -c boot/start.S
	$(CROSS)gcc $(CFLAGS) -c boot/trap_entry.S
	$(CROSS)gcc $(CFLAGS) -c drivers/block.c
	$(CROSS)gcc $(CFLAGS) -c kernel/context.S
	$(CROSS)gcc $(CFLAGS) -c kernel/fs.c
	$(CROSS)gcc $(CFLAGS) -c kernel/inode.c
	$(CROSS)gcc $(CFLAGS) -c kernel/main.c
	$(CROSS)gcc $(CFLAGS) -c kernel/memory.c
	$(CROSS)gcc $(CFLAGS) -c kernel/scheduler.c
	$(CROSS)gcc $(CFLAGS) -c kernel/string.c
	$(CROSS)gcc $(CFLAGS) -c kernel/task.c
	$(CROSS)gcc $(CFLAGS) -c kernel/timestamp.c
	$(CROSS)gcc $(CFLAGS) -c kernel/treefs.c
	$(CROSS)gcc $(CFLAGS) -c kernel/uart.c

	$(CROSS)ld -T linker.ld $(OBJS) -o kernel.elf