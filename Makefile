

# Makefile for the first-stage boot loader

ASM := nasm
ASMFLAGS := -f bin -i ./util/
ASMFLAGSL := -f elf32 -i ./util/
LINK := ld
LINKFLAGS := -T ./kernel/linker.ld
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
LOADER_OBJECTS := $(OBJECT_DIR)loader/main.o 
LOADER_IMAGE := $(OUTPUT_DIR)loader.bin

# the OS kernel files
KERNEL_DIR := ./kernel/
KERNEL_C_SOURCES := $(shell find $(KERNEL_DIR) -name '*.c')
KERNEL_ASM_SOURCES := $(shell find $(KERNEL_DIR) -name '*.asm')

KERNEL_C_OBJECTS := $(patsubst $(KERNEL_DIR)%, $(OBJECT_DIR)kernel/%, $(addsuffix .o, $(basename $(KERNEL_C_SOURCES))))
KERNEL_ASM_OBJECTS := $(patsubst $(KERNEL_DIR)%, $(OBJECT_DIR)kernel/%, $(addsuffix .o, $(basename $(KERNEL_ASM_SOURCES))))

KERNEL_OBJECTS := $(KERNEL_C_OBJECTS)
KERNEL_OBJECTS += $(KERNEL_ASM_OBJECTS)

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
$(LOADER_OBJECTS): $(LOADER_SOURCES)
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADER_IMAGE): $(LOADER_OBJECTS)
	cp $(LOADER_OBJECTS) $(LOADER_IMAGE)

$(info $(CCFLAGS))
# the kernel rules
$(OBJECT_DIR)kernel/%.o: $(KERNEL_DIR)%.c 
	$(CC) $(CCFLAGS) $< -o $@
$(OBJECT_DIR)kernel/%.o: $(KERNEL_DIR)%.asm 
	$(ASM) $(ASMFLAGSL) $< -o $@

# we force ke_main.o to be the first object linked
$(KERNEL_IMAGE): $(KERNEL_OBJECTS) $(UTIL_OBJECTS)
	$(LINK) $(LINKFLAGS) -o $@ $(KERNEL_OBJECTS) $(UTIL_OBJECTS)

# the final ruleset
$(TARGET): $(BOOT_IMAGE) $(LOADER_IMAGE) $(KERNEL_IMAGE)
	python ./drv/main.py -f $(BOOT_IMAGE) -s $(LOADER_IMAGE) $(KERNEL_IMAGE) -o $(OUTPUT_FILE)
    




