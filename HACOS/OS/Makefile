# Build a 64-bit UEFI app and run it in QEMU with OVMF.

TARGET   := BOOTX64.EFI
BUILD    := build
ESP_DIR  := $(BUILD)/esp
EFI_DIR  := $(ESP_DIR)/EFI/BOOT
IMG      := $(BUILD)/esp.img

CC       := gcc
CFLAGS   := -ffreestanding -fno-stack-protector -fshort-wchar -fpic \
            -mno-red-zone -Wall -Wextra -I. -I/usr/include/efi -I/usr/include/efi/x86_64
LDFLAGS  := -nostdlib -znocombreloc -shared -Bsymbolic

# Adjust these paths if they differ on your distro
GNUEFI_DIR  := /usr/lib/x86_64-linux-gnu/gnuefi
EFI_LDS     := $(GNUEFI_DIR)/elf_x86_64_efi.lds
EFI_CRT0    := $(GNUEFI_DIR)/crt0-efi-x86_64.o
LIBDIRS     := -L/usr/lib -L$(GNUEFI_DIR)
LIBS        := -lefi -lgnuefi

OVMF_CODE := /usr/share/OVMF/OVMF_CODE.fd
OVMF_VARS := /usr/share/OVMF/OVMF_VARS.fd

all: $(BUILD)/$(TARGET) image

$(BUILD):
    mkdir -p $(BUILD)

$(BUILD)/main.o: main.c | $(BUILD)
    $(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/app.so: $(BUILD)/main.o
    ld $(LDFLAGS) -T $(EFI_LDS) $(EFI_CRT0) $< $(LIBDIRS) $(LIBS) -o $@

$(BUILD)/$(TARGET): $(BUILD)/app.so
    objcopy \
      -j .text -j .sdata -j .data -j .dynamic -j .dynsym \
      -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
      --target=efi-app-x86_64 $< $@

image: $(BUILD)/$(TARGET)
    mkdir -p $(EFI_DIR)
    cp $(BUILD)/$(TARGET) $(EFI_DIR)/$(TARGET)
    # Build a fresh 64MB FAT image and copy the EFI app to the default boot path
    dd if=/dev/zero of=$(IMG) bs=1M count=64 status=none
    mkfs.vfat -F32 $(IMG)
    mmd   -i $(IMG) ::/EFI ::/EFI/BOOT
    mcopy -i $(IMG) $(EFI_DIR)/$(TARGET) ::/EFI/BOOT/$(TARGET)

run: image
    cp $(OVMF_VARS) $(BUILD)/OVMF_VARS.fd
    qemu-system-x86_64 \
      -machine q35 -cpu qemu64 -m 256 \
      -drive if=pflash,format=raw,unit=0,readonly=on,file=$(OVMF_CODE) \
      -drive if=pflash,format=raw,unit=1,file=$(BUILD)/OVMF_VARS.fd \
      -drive if=ide,format=raw,file=$(IMG)

clean:
    rm -rf $(BUILD)

.PHONY: all image run clean
