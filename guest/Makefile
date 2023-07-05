BUILD_DIR = ./build

SRCS_ASM = $(shell find ./ -name "*.S")
SRCS_C = $(shell find ./ -name "*.c")

OBJS = $(SRCS_ASM:%.S=$(BUILD_DIR)/%.o)
OBJS += $(SRCS_C:%.c=$(BUILD_DIR)/%.o)

TARGET_NAME = guest
TARGET = $(TARGET_NAME).elf
TARGET_BIN = $(TARGET_NAME).bin

CROSS_COMPILE = riscv64-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -march=rv64im -mabi=lp64 -mcmodel=medany -g -Wall
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

build:$(OBJS)
	$(CC) $(CFLAGS) -T linker.ld -o $(TARGET) $^
	$(OBJCOPY) $(TARGET) --strip-all -O binary $(TARGET_BIN)

$(BUILD_DIR)/%.o: %.c 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.S 
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm ./build -rf