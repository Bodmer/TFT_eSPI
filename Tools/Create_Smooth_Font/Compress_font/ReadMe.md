### VLW Compress for TFT_eSPI

Used to transform 8bit VLW file into 4bit VLW file with no padding support.

**Advantage**:  It will save at least 50% space.

**Limitation**: Fonts that height over 255 are not being generated properly.

To Compile,run the command below:
    
Windows(MinGW needed): `gcc vlwcompress.c -o vlwcompress.exe`
Linux  (GCC needed):   `gcc vlwcompress.c -o vlwcompress`
	
	
#### Compressed Font format 


Header of vlw file is the same as uncompressed;it comprises 6 uint32_t parameters (24 bytes total):
      
1. The gCount (number of character glyphs)
2. A version number (0xB = 11 for uncompressed,0XC = 12 for compressed 4bit)
3. The font size (in points, not pixels)
4. Deprecated mboxY parameter (typically set to 0)
5. Ascent in pixels from baseline to top of "d"
6. Descent in pixels from baseline to bottom of "p"

Next are gCount sets of values for each glyph(8 bytes):
      
1. (16 bits) Glyph Unicode
2. (8 bits)  Height of bitmap bounding box
3. (8 bits)  Width of bitmap bounding box   
4. (8 bits)  gxAdvance for cursor (setWidth in Processing)
5. (16 bits) dY = distance from cursor baseline to top of glyph bitmap (signed value +ve = up)
6. (8 bits)  dX = distance from cursor to left side of glyph bitmap (signed value -ve = left)

The bitmaps start next at 24 + (8 * gCount) bytes from the start of the file.
Each pixel is 0.5 byte, an 4 bit Alpha value which represents the transparency from 
0xF foreground colour, 0x0 background.When a line consist of odd number of pixels,the last byte 
for the last pixel of the line will be filled with 0 as low 4 bits.
4 bits alpha value will be converted to 8 bits before drawing.

After the bitmaps is (Not modified):

- 1 byte for font name string length (excludes null)
- a zero terminated character string giving the font name
- 1 byte for Postscript name string length
- a zero/one terminated character string giving the font name
- last byte is 0 for non-anti-aliased and 1 for anti-aliased (smoothed)
