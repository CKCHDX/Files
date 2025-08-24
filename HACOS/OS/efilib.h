#ifndef EFILIB_H
#define EFILIB_H
#include "efi.h"

#define EFI_SUCCESS 0

// Simplified EFI entry point macro
#define EFIAPI __attribute__((ms_abi))

// Minimal console output protocol
typedef struct {
    EFI_STATUS (*OutputString)(void *This, CHAR16 *String);
} SIMPLE_TEXT_OUTPUT_INTERFACE;

#endif
