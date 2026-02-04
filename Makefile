

# Makefile for the first-stage boot loader

ASM := nasm
ASMFLAGS := -f bin -i ./util/
ASMFLAGSL := -f coff -i ./util/
LINK := ld
LINKFLAGS := -T ./kernel/linker.ld -m elf_i386 --oformat binary 
CC := gcc
CCFLAGS := -I. -I./kernel -fno-stack-protector -ffreestanding -nostdlib -m32 -fno-pie -c

# The final boot image
TARGET := bootimage
OUTPUT_DIR := ./bin/
OBJECT_DIR := ./obj/
OUTPUT_FILE := $(OUTPUT_DIR)$(TARGET)

# the bootloader files
BOOT_DIR := ./boot/
BOOT_SOURCES := $(BOOT_DIR)boot.asm
BOOT_IMAGE := $(OUTPUT_DIR)boot.bin

# the OS loader files
LOADER_DIR := ./loader/
LOADER_SOURCES := $(LOADER_DIR)main.asm
LOADER_OBJECTS := $(patsubst $(LOADER_DIR)%.asm,$(OBJECT_DIR)%.o,$(LOADER_SOURCES))
LOADER_OBJECTS := $(patsubst $(LOADER_DIR)%.c,$(OBJECT_DIR)%.o,$(LOADER_OBJECTS))
LOADER_IMAGE := $(OUTPUT_DIR)loader.bin

# the OS kernel files
KERNEL_DIR := ./kernel/
KERNEL_SOURCES := $(wildcard $(KERNEL_DIR)*.c)
KERNEL_SOURCES += $(wildcard $(KERNEL_DIR)modules/fs/*.c)
KERNEL_SOURCES += $(wildcard $(KERNEL_DIR)*.asm)
KERNEL_OBJECTS := $(patsubst $(KERNEL_DIR)%.c,$(OBJECT_DIR)kernel/%.o,$(KERNEL_SOURCES))
KERNEL_OBJECTS := $(patsubst $(KERNEL_DIR)modules/fs/%.c,$(OBJECT_DIR)kernel/modules/%.o,$(KERNEL_OBJECTS))
KERNEL_OBJECTS := $(patsubst $(KERNEL_DIR)%.asm,$(OBJECT_DIR)kernel/%.o,$(KERNEL_OBJECTS))
KERNEL_IMAGE := $(OUTPUT_DIR)kernel.bin

# the util images
UTIL_DIR := ./util/
UTIL_SOURCES := $(wildcard $(UTIL_DIR)*.asm)
UTIL_OBJECTS := $(patsubst $(UTIL_DIR)%.asm,$(OBJECT_DIR)util/%.o,$(UTIL_SOURCES))
 
OBJECTS += $(LOADER_OBJECTS) $(KERNEL_OBJECTS) $(UTIL_OBJECTS)
IMAGES += $(BOOT_IMAGE) $(LOADER_IMAGE) $(KERNEL_IMAGE)

all: $(TARGET)

clean:
	rm $(IMAGES) $(OBJECTS) $(OUTPUT_FILE)

# util rules
$(OBJECT_DIR)util/%.o: $(UTIL_DIR)%.asm
	$(ASM) $(ASMFLAGSL) $< -o $@

# bootloader rules
$(OUTPUT_DIR)%.bin: $(BOOT_DIR)%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# the OS loader rules
$(OBJECT_DIR)%.o: $(LOADER_DIR)%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJECT_DIR)%.o: $(LOADER_DIR)%.c
	$(CC) $(CCFLAGS) $< -o $@	

$(LOADER_IMAGE): $(LOADER_OBJECTS)
	# $(LINK) $(LINKFLAGS) -o $(LOADER_IMAGE) $(LOADER_OBJECTS)
	cp $(LOADER_OBJECTS) $(LOADER_IMAGE)

# the kernel rules
$(OBJECT_DIR)kernel/%.o: $(KERNEL_DIR)%.c
	$(CC) $(CCFLAGS) $< -o $@
$(OBJECT_DIR)kernel/modules/fs/%.o: $(KERNEL_DIR)modules/fs/%.c
	$(CC) $(CCFLAGS) $< -o $@

$(OBJECT_DIR)kernel/%.o: $(KERNEL_DIR)%.asm
	$(ASM) $(ASMFLAGSL) $< -o $@

$(KERNEL_IMAGE): $(KERNEL_OBJECTS) $(UTIL_OBJECTS)
	$(LINK) $(LINKFLAGS) -o $@ $(KERNEL_OBJECTS) $(UTIL_OBJECTS)

# the final ruleset
$(TARGET): $(BOOT_IMAGE) $(LOADER_IMAGE) $(KERNEL_IMAGE)
	python ./drv/main.py -f $(BOOT_IMAGE) -s $(LOADER_IMAGE) $(KERNEL_IMAGE) -o $(OUTPUT_FILE)
    




