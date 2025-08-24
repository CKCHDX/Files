#include "efi.h"
#include "efilib.h"

// Minimal 8x16 glyphs for 'h','e','l','o',' ' (space). 1-bit per pixel, MSB left.
static const UINT8 glyph_h[16] = {
  0x00,0x00,0x82,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x82,0x82,0x00,0x00,0x00,0x00
};
static const UINT8 glyph_e[16] = {
  0x00,0x00,0x7C,0x82,0x80,0xF8,0x80,0x80,0x82,0x7C,0x00,0x00,0x00,0x00,0x00,0x00
};
static const UINT8 glyph_l[16] = {
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,0x7C,0x00,0x00,0x00,0x00,0x00
};
static const UINT8 glyph_o[16] = {
  0x00,0x00,0x7C,0x82,0x82,0x82,0x82,0x82,0x82,0x7C,0x00,0x00,0x00,0x00,0x00,0x00
};
static const UINT8 glyph_space[16] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const UINT8* get_glyph(CHAR16 ch) {
  switch (ch) {
    case L'h': return glyph_h;
    case L'e': return glyph_e;
    case L'l': return glyph_l;
    case L'o': return glyph_o;
    case L' ': return glyph_space;
    default:   return glyph_space; // fallback to blank for unknown chars
  }
}

typedef struct {
  UINT8 r, g, b;
} RGB;

static inline VOID write_pixel(
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop,
  UINT32 x, UINT32 y, RGB c
) {
  UINT32 ppsl = gop->Mode->Info->PixelsPerScanLine;
  UINT8 *fb   = (UINT8*)(UINTN)gop->Mode->FrameBufferBase;
  UINTN idx   = ((UINTN)y * ppsl + x) * 4;

  switch (gop->Mode->Info->PixelFormat) {
    case PixelBlueGreenRedReserved8BitPerColor: // BGRA
      fb[idx+0] = c.b;
      fb[idx+1] = c.g;
      fb[idx+2] = c.r;
      fb[idx+3] = 0x00;
      break;
    case PixelRedGreenBlueReserved8BitPerColor: // RGBA
      fb[idx+0] = c.r;
      fb[idx+1] = c.g;
      fb[idx+2] = c.b;
      fb[idx+3] = 0x00;
      break;
    case PixelBitMask:
      // Simplistic RGBA write assuming 8:8:8:8 masks (common on OVMF)
      fb[idx+0] = c.b;
      fb[idx+1] = c.g;
      fb[idx+2] = c.r;
      fb[idx+3] = 0x00;
      break;
    default:
      // PixelBltOnly etc. — do nothing
      break;
  }
}

static VOID fill_rect(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop,
                      UINT32 x, UINT32 y, UINT32 w, UINT32 h, RGB c) {
  for (UINT32 yy = 0; yy < h; ++yy) {
    for (UINT32 xx = 0; xx < w; ++xx) {
      write_pixel(gop, x + xx, y + yy, c);
    }
  }
}

static VOID draw_char(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop,
                      UINT32 x, UINT32 y, CHAR16 ch, RGB fg, RGB bg,
                      UINT32 scale /* integer scale factor */) {
  const UINT8 *g = get_glyph(ch);
  for (UINT32 row = 0; row < 16; ++row) {
    UINT8 bits = g[row];
    for (UINT32 col = 0; col < 8; ++col) {
      RGB color = (bits & (0x80 >> col)) ? fg : bg;
      if (scale == 1) {
        write_pixel(gop, x + col, y + row, color);
      } else {
        fill_rect(gop, x + col*scale, y + row*scale, scale, scale, color);
      }
    }
  }
}

static VOID draw_string(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop,
                        UINT32 x, UINT32 y, const CHAR16 *s,
                        RGB fg, RGB bg, UINT32 scale) {
  UINT32 cx = x;
  while (*s) {
    draw_char(gop, cx, y, *s, fg, bg, scale);
    cx += (8 * scale) + scale; // 1px space between glyphs
    ++s;
  }
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  InitializeLib(ImageHandle, SystemTable);

  // Locate the Graphics Output Protocol (GOP)
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
  EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

  EFI_STATUS st = uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);
  if (EFI_ERROR(st) || !gop || !gop->Mode || !gop->Mode->Info) {
    Print(L"Unable to locate GOP (%r). Falling back to text.\n", st);
    Print(L"hello\n");
    return EFI_SUCCESS;
  }

  // Optionally pick a graphics mode (keep current mode if valid)
  // Here we just use the current mode for simplicity.

  UINT32 W = gop->Mode->Info->HorizontalResolution;
  UINT32 H = gop->Mode->Info->VerticalResolution;

  // Clear screen to dark blue
  fill_rect(gop, 0, 0, W, H, (RGB){ .r = 0x00, .g = 0x00, .b = 0x40 });

  // Draw a panel area
  UINT32 pw = (W * 3) / 4;
  UINT32 ph = (H * 1) / 3;
  UINT32 px = (W - pw)/2;
  UINT32 py = (H - ph)/2;
  fill_rect(gop, px, py, pw, ph, (RGB){ .r = 0xF0, .g = 0xF0, .b = 0xF0 });

  // Draw "hello" centered-ish
  const CHAR16 *msg = L"hello";
  UINT32 scale = 3; // make glyphs bigger (8x16 -> 24x48)
  UINT32 text_w = 5 * (8*scale + scale) - scale; // 5 chars total in "hello"
  UINT32 text_h = 16 * scale;

  UINT32 tx = px + (pw - text_w)/2;
  UINT32 ty = py + (ph - text_h)/2;

  draw_string(gop, tx, ty, msg,
              (RGB){ .r = 0x20, .g = 0x20, .b = 0x20 },   // fg: dark gray
              (RGB){ .r = 0xF0, .g = 0xF0, .b = 0xF0 },   // bg: panel
              scale);

  // Wait a few seconds so you can see it in QEMU
  uefi_call_wrapper(BS->Stall, 1, 5 * 1000 * 1000);

  return EFI_SUCCESS;
}
