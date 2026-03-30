
# global settings
CC := gcc
ASM := nasm

CC_FLAGS := -c -Wall -Wextra -Werror -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -fno-pie -mno-red-zone -I.
ASM_FLAGS := -f bin  
ASM_CFLAGS := -f elf64

LINK := ld

OUT_DIR := build/
OBJ_DIR := build/obj/
OUT_IMAGE := $(OUT_DIR)out_image

# first stage bootloader
MBR_SRCS := mbr/boot.asm
MBR_OUT_FILE := $(OUT_DIR)MBR

# second stage bootloader
BOOTLDR_SRCS := bootldr/main.asm
BOOTLDR_OUT_FILE := $(OUT_DIR)bootldr

# OS loader
OSLDR_C_SRCS := bootldr/osldr/main.c
OSLDR_C_OBJS := $(OBJ_DIR)bootldr/osldr/main.o 
OSLDR_ASM_SRCS := bootldr/osldr/proc.asm
OSLDR_ASM_OBJS := $(OBJ_DIR)bootldr/osldr/proc.o

OSLDR_LINKER_FILE := bootldr/osldr/linker.ld

OSLDR_SRC_DIR := bootldr/osldr
OSLDR_OBJ_DIR := $(OBJ_DIR)bootldr/osldr
OSLDR_OUT_FILE := $(OUT_DIR)osldr

# kernel
KERNEL_C_SRCS := $(shell find kernel/ -name '*.c')
KERNEL_C_OBJS := $(patsubst kernel/%.c, $(OBJ_DIR)kernel/%.o, $(KERNEL_C_SRCS)) 

KERNEL_ASM_SRCS := $(shell find kernel/ -name '*.asm')
KERNEL_ASM_OBJS := $(patsubst kernel/%.asm, $(OBJ_DIR)kernel/%.o, $(KERNEL_ASM_SRCS)) 

KERNEL_LINKER_FILE := kernel/linker.ld

KERNEL_SRC_DIR := kernel
KERNEL_OBJ_DIR := $(OBJ_DIR)kernel
KERNEL_OUT_FILE := $(OUT_DIR)kernel

# targets
all: $(OUT_IMAGE) 

$(OUT_IMAGE): $(MBR_OUT_FILE) $(BOOTLDR_OUT_FILE) $(OSLDR_OUT_FILE) $(KERNEL_OUT_FILE)
	python drv/main.py -f $(MBR_OUT_FILE) -s $(BOOTLDR_OUT_FILE) $(OSLDR_OUT_FILE) $(KERNEL_OUT_FILE) -o $@

$(MBR_OUT_FILE): $(MBR_SRCS)
	$(ASM) $(ASM_FLAGS) $< -o $(MBR_OUT_FILE) 

$(BOOTLDR_OUT_FILE): $(BOOTLDR_SRCS)
	$(ASM) $(ASM_FLAGS) -I ./bootldr/ $< -o $@

$(OSLDR_OBJ_DIR)/%.o: $(OSLDR_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -I./bootldr/osrldr/ $< -o $@
$(OSLDR_OBJ_DIR)/%.o: $(OSLDR_SRC_DIR)/%.asm
	mkdir -p $(dir $@)
	$(ASM) $(ASM_CFLAGS) -I ./bootldr -I./bootldr/osldr/ $< -o $@

$(OSLDR_OUT_FILE): $(OSLDR_C_OBJS) $(OSLDR_ASM_OBJS)
	$(LINK) -T$(OSLDR_LINKER_FILE) -o $(OSLDR_OUT_FILE) $^

$(KERNEL_OBJ_DIR)/%.o: $(KERNEL_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -I./kernel $< -o $@

$(KERNEL_OBJ_DIR)/%.o: $(KERNEL_SRC_DIR)/%.asm
	mkdir -p $(dir $@)
	$(ASM) $(ASM_CFLAGS) $< -o $@

$(KERNEL_OUT_FILE): $(KERNEL_C_OBJS) $(KERNEL_ASM_OBJS)
	$(LINK) -T$(KERNEL_LINKER_FILE) -o $@ $^

clean:
	rm $(OUT_IMAGE)
	rm $(MBR_OUT_FILE)
	rm $(BOOTLDR_OUT_FILE)
	rm $(OSLDR_C_OBJS) $(OSLDR_ASM_OBJS) $(OSLDR_OUT_FILE)
	rm $(KERNEL_C_OBJS) $(KERNEL_ASM_OBJS) $(KERNEL_OUT_FILE)


