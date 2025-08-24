# Build a 64-bit UEFI application using gnu-efi, package paths from Ubuntu.
# Produces BOOTX64.EFI, creates a FAT image, and runs it in QEMU with OVMF.

TARGET   := BOOTX64.EFI
BUILD    := build
IMG      := disk.img

EFIINC   := /usr/include/efi
EFILIB64 := /usr/lib/x86_64-linux-gnu
GNUEFI   := $(EFILIB64)/gnuefi

CC       := gcc
CFLAGS   := -I$(EFIINC) -I$(EFIINC)/x86_64 -fshort-wchar -fno-stack-protector -fpic \
            -fno-omit-frame-pointer -ffreestanding -mno-red-zone -Wall -Wextra -O2
LDSCRIPT := $(GNUEFI)/elf_x86_64_efi.lds
CRT0     := $(EFILIB64)/crt0-efi-x86_64.o
LIBS     := -L$(EFILIB64) -lgnuefi -lefi
LDFLAGS  := -nostdlib -znocombreloc -shared -Bsymbolic -T $(LDSCRIPT)

QEMU     := qemu-system-x86_64
OVMF_CODE:= /usr/share/OVMF/OVMF_CODE.fd
OVMF_VARS:= OVMF_VARS.fd

SRC      := main.c
OBJ      := $(BUILD)/main.o
SO       := $(BUILD)/app.so

.PHONY: all clean image run vars

all: $(TARGET)

$(BUILD):
    mkdir -p $(BUILD)

$(OBJ): $(SRC) efi.h efilib.h | $(BUILD)
    $(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

$(SO): $(OBJ)
    ld $(LDFLAGS) $(CRT0) $(OBJ) $(LIBS) -o $(SO)

$(TARGET): $(SO)
    # Strip and convert sections to the final PE/COFF EFI format
    objcopy \
        -j .text -j .sdata -j .data -j .dynamic -j .dynsym \
        -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
        --target=efi-app-x86_64 $(SO) $(TARGET)

image: $(TARGET)
    # Create a 64MB FAT32 disk image and copy BOOTX64.EFI to EFI/BOOT
    rm -f $(IMG)
    truncate -s 64M $(IMG)
    mkfs.vfat -F 32 $(IMG)
    mmd   -i $(IMG) ::/EFI
    mmd   -i $(IMG) ::/EFI/BOOT
    mcopy -i $(IMG) $(TARGET) ::/EFI/BOOT/BOOTX64.EFI

vars:
    # Make a writable copy of OVMF_VARS for QEMU run
    cp -f /usr/share/OVMF/OVMF_VARS.fd $(OVMF_VARS)

run: image vars
    $(QEMU) \
        -nographic \
        -machine q35,accel=kvm:tcg \
        -drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
        -drive if=pflash,format=raw,file=$(OVMF_VARS) \
        -drive format=raw,file=$(IMG)

clean:
    rm -rf $(BUILD) $(TARGET) $(SO) $(IMG) $(OVMF_VARS)
