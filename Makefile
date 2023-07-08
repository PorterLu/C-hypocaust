CPUS	:= 1
QEMU	:= qemu-system-riscv64
BOOTLOADER := bootloader/opensbi-qemu.bin

KERNEL_ENTRY_PA	:= 0x80200000

QEMUOPTS	= -M virt -m 512M -bios $(BOOTLOADER) -nographic -smp $(CPUS)
QEMUOPTS	 += -device loader,file=./$(TARGET_BIN),addr=$(KERNEL_ENTRY_PA) 

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
CFLAGS = -nostdlib -fno-builtin -march=rv64g -mabi=lp64 -mcmodel=medany -g -Wall -I./include
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

.PHONY: build clean qemu

build: $(OBJS)
	$(CC) $(CFLAGS) -T src/linker.ld -o $(TARGET) $^
	$(OBJCOPY) $(TARGET) --strip-all -O binary $(TARGET_BIN)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.S
	@make -C guest build
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

qemu: build
	$(QEMU) $(QEMUOPTS)
	
debug: build
	$(QEMU) $(QEMUOPTS) -s -S

clean:
	-@rm $(BUILD_DIR) -rf
	-@rm $(TARGET) $(TARGET_BIN)
	-@make -C guest clean

disasm: build
	$(OBJDUMP) -d $(TARGET) | less
