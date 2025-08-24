#include "efi.h"
#include "efilib.h"

// GOP GUID for LocateProtocol
static EFI_GUID gopGuid = {0x9042a9de,0x23dc,0x4a38,{0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}};

static inline UINT32 rgb(UINT8 r, UINT8 g, UINT8 b) {
    // UEFI PixelBlueGreenRedReserved8BitPerColor (least significant byte is Blue)
    return ((UINT32)b) | ((UINT32)g << 8) | ((UINT32)r << 16);
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    InitializeLib(ImageHandle, SystemTable);

    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
    EFI_STATUS st = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(st) || !gop || !gop->Mode || !gop->Mode->Info) {
        Print(L"Failed to locate GOP: %r\r\n", st);
        return st;
    }

    UINT32 w = gop->Mode->Info->HorizontalResolution;
    UINT32 h = gop->Mode->Info->VerticalResolution;
    UINT32 stride = gop->Mode->Info->PixelsPerScanLine;
    UINT32* fb = (UINT32*)(UINTN)gop->Mode->FrameBufferBase;

    // Paint a blue background
    UINT32 color = rgb(0x10, 0x20, 0xB0);
    for (UINT32 y = 0; y < h; ++y) {
        UINT32* row = fb + (UINTN)y * stride;
        for (UINT32 x = 0; x < w; ++x) {
            row[x] = color;
        }
    }

    // Put the firmware text console on top (it renders in graphics mode)
    ST->ConOut->SetAttribute(ST->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLUE);
    ST->ConOut->ClearScreen(ST->ConOut);

    // Center-ish “Hello”
    CHAR16* msg = L"Hello";
    UINTN cols, rows;
    ST->ConOut->QueryMode(ST->ConOut, ST->ConOut->Mode->Mode, &cols, &rows);
    UINTN len = 5;
    UINTN col = (cols > len) ? (cols - len) / 2 : 0;
    UINTN row = rows / 2;
    ST->ConOut->SetCursorPosition(ST->ConOut, col, row);
    ST->ConOut->OutputString(ST->ConOut, msg);
    ST->ConOut->SetCursorPosition(ST->ConOut, col, row + 2);
    ST->ConOut->OutputString(ST->ConOut, L"Press any key to exit...");

    // Wait for a key press before returning to firmware
    UINTN idx = 0;
    uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &idx);
    EFI_INPUT_KEY key;
    ST->ConIn->ReadKeyStroke(ST->ConIn, &key);

    return EFI_SUCCESS;
}
