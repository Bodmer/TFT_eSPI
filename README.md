This is a branch of https://github.com/Bodmer/TFT_eSPI.git

It is created to solve some problems I encountered while using Bodmer's lib in my mp3 player based on ESP32:
1) I can only use smooth fonts to display unicode font with about 13K glyphs.
2) The font can be placed in array, but then application uploading time becomes very long
3) The font can be placed in SPIFFS, but then printing time is unacceptable
4) The font loading takes huge amount of time and RAM
5) Printing chars with high codes is much slower than chars from basic ASCII range.

This version of TFT_eSPI solves those problems:
1) The font can be loaded in separate partition in flash, so there is no need to re-upload it with every code upload.
2) The font format (vlw) is converted to custom format (prt), which is:
  - all metrics are little-endian to match the CPU = no need to swap bytes
  - contains pre-calculated glyph offsets = no need to scan font for glyph sizes
  - contains pre-calculated max_descent = no need to scan font for max_descent
3) Glyph metrics is loaded on-the-fly when printing, so no RAM is used
4) Faster getUnicodeIndex function (FAST_UNICODE_INDEX) = glyph searching time is greatly decreased in large font

Using partition for font is somewhat strange:
- I can only use SPI_FLASH_MMAP_INST - instruction memory mapping, not data memory. And it forces to use 32-bit aligned access only. 
- Partition font seems a little slower than array font, but in final version of code usual array font can be used.

This code is tested in my player only, so it can contain bugs and incompatibilities.

Known problems:
1. When program size grows and when using font partition, some characters stop printing as expected (very high codes), 0xBAD00BAD value is readed from flash instead of bitmap data.

