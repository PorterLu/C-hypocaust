CPUS	:= 1
QEMU	:= qemu-system-riscv64
BOOTLOADER := bootloader/opensbi-qemu.bin

KERNEL_ENTRY_PA	:= 0x80200000

QEMUOPTS	= -M virt -m 2G -bios $(BOOTLOADER) -nographic -smp $(CPUS)
QEMUOPTS 	+= -device loader,file=./$(TARGET_BIN),addr=$(KERNEL_ENTRY_PA) 

SRCS_ASM = $(shell find ./src -name "*.S")
SRCS_C = $(shell find ./src -name "*.c")

BUILD_DIR = ./build
OBJS = $(SRCS_ASM:%.S=$(BUILD_DIR)/%.o)
OBJS += $(SRCS_C:%.c=$(BUILD_DIR)/%.o)

INC_PATH = ./include

TARGET_NAME = hyper
TARGET = $(TARGET_NAME).elf
TARGET_BIN = $(TARGET_NAME).bin

CROSS_COMPILE = riscv64-unknown-elf-
CFLGAS = -nostdlib -fno-builtin -march=rv64im -mabi=lp64 -mcmodel=medany -g -Wall -I./include
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

build:$(OBJS)
	$(CC) $(CFLGAS) -T src/linker.ld -o $(TARGET) $^
	$(OBJCOPY) $(TARGET) --strip-all -O binary $(TARGET_BIN)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLGAS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLGAS) -c -o $@ $<

qemu: build
	$(QEMU) $(QEMUOPTS)
	
debug: build
	$(QEMU) $(QEMUOPTS) -s -S

clean:
	@rm $(BUILD_DIR) -rf
	@rm $(TARGET) $(TARGET_BIN)

disasm:
	$(OBJDUMP) -d $(TARGET) | less