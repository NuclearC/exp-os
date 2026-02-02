

# Makefile for the first-stage boot loader

ASM := nasm
ASMFLAGS := -f bin -i ./util/
ASMFLAGSL := -f coff
LINK := ld
LINKFLAGS := -Ttext 0x1000 --oformat binary 
CC := gcc
CCFLAGS := -ffreestanding -c

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
$(info  $(LOADER_OBJECTS))
LOADER_IMAGE := $(OUTPUT_DIR)loader.bin

OBJECTS += $(LOADER_OBJECTS)
IMAGES += $(BOOT_IMAGE) $(LOADER_IMAGE)

all: $(TARGET)

clean:
	rm $(IMAGES) $(OBJECTS) $(OUTPUT_FILE)

$(OUTPUT_DIR)%.bin: $(BOOT_DIR)%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJECT_DIR)%.o: $(LOADER_DIR)%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(OBJECT_DIR)%.o: $(LOADER_DIR)%.c
	$(CC) $(CCFLAGS) $< -o $@	

$(LOADER_IMAGE): $(LOADER_OBJECTS)
	# $(LINK) $(LINKFLAGS) -o $(LOADER_IMAGE) $(LOADER_OBJECTS)
	cp $(LOADER_OBJECTS) $(LOADER_IMAGE)

$(TARGET): $(BOOT_IMAGE) $(LOADER_IMAGE)
	python ./drv/main.py -f $(OUTPUT_DIR)boot.bin -s $(OUTPUT_DIR)loader.bin -o $(OUTPUT_FILE)
    




