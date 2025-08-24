TARGET = BOOTX64.EFI
ISO = os.iso
CC = x86_64-w64-mingw32-gcc
LD = x86_64-w64-mingw32-ld
CFLAGS = -ffreestanding -fno-stack-protector -fshort-wchar -mno-red-zone -Wall -Wextra
LDFLAGS = -nostdlib -Wl,--subsystem,10 -T linker.ld

all: $(ISO)

$(ISO): $(TARGET)
	mkdir -p iso/EFI/BOOT
	cp $(TARGET) iso/EFI/BOOT/BOOTX64.EFI
	xorriso -as mkisofs -R -J -e EFI/BOOT/BOOTX64.EFI \
	    -no-emul-boot -o $(ISO) iso

$(TARGET): main.o
	$(LD) $(LDFLAGS) -o $@ $^

main.o: main.c efi.h efilib.h
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -rf *.o $(TARGET) $(ISO) iso
