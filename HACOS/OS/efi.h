#ifndef EFI_H
#define EFI_H

#include <stdint.h>

typedef uint64_t EFI_STATUS;
typedef void *EFI_HANDLE;
typedef uint16_t CHAR16;

typedef struct {
    void *ConOut; // Simple console output (for early testing)
} EFI_SYSTEM_TABLE;

#endif
