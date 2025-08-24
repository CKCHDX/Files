#include "efi.h"
#include "efilib.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    SIMPLE_TEXT_OUTPUT_INTERFACE *ConOut = (SIMPLE_TEXT_OUTPUT_INTERFACE*)SystemTable->ConOut;
    ConOut->OutputString(SystemTable->ConOut, L"Hello from UEFI!\r\n");
    return EFI_SUCCESS;
}
