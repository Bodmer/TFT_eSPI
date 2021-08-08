 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the TFT_eSPI class and is associated with anti-aliased font functions

 public:

  // These are for the new antialiased fonts
  void     loadFont(const uint8_t array[]);
#ifdef FONT_FS_AVAILABLE
  void     loadFont(String fontName, fs::FS &ffs);
#endif
  void     loadFont(String fontName, bool flash = true);
  void     loadFont(const char * partitionName, esp_partition_subtype_t subtype);

  void     unloadFont( void );
  bool     getUnicodeIndex(uint16_t unicode, uint16_t *index);

  virtual void drawGlyph(uint16_t code);

  void     showFont(uint32_t td);

 // This is for the whole font
  typedef struct
  {
    const uint8_t* gArray;           //array start pointer
    uint16_t gCount;                 // Total number of characters
    uint16_t yAdvance;               // Line advance
    uint16_t spaceWidth;             // Width of a space character
    int16_t  ascent;                 // Height of top of 'd' above baseline, other characters may be taller
    int16_t  descent;                // Offset to bottom of 'p', other characters may have a larger descent
    uint16_t maxAscent;              // Maximum ascent found in font
    uint16_t maxDescent;             // Maximum descent found in font
  } fontMetrics;

  fontMetrics gFont = { nullptr, 0, 0, 0, 0, 0, 0, 0 };

#define MAX_GLYPH_WIDTH 30
    uint8_t glyph_line_buffer[MAX_GLYPH_WIDTH];

  // These are for the metrics for each individual glyph (so we don't need to seek this in file and waste time)
  typedef struct
  {
    uint16_t gUnicode;  //UTF-16 code, the codes are searched so do not need to be sequential
    uint8_t  gHeight;   //cheight
    uint8_t  gWidth;    //cwidth
    uint8_t  gxAdvance; //setWidth
    int16_t  gdY;       //topExtent
    int8_t   gdX;       //leftExtent
    uint32_t gBitmap;   //bitmap offset
  } CharMetrics;
  
  TFT_eSPI::CharMetrics * getCharMetrics(uint16_t gNum);
 
//  uint32_t* gBitmap = NULL;   //file pointer to greyscale bitmap

  bool     fontLoaded = false; // Flags when a anti-aliased font is loaded

  spi_flash_mmap_handle_t font_handle;
  
#ifdef FONT_FS_AVAILABLE
  fs::File fontFile;
  fs::FS   &fontFS  = SPIFFS;
  bool     spiffs   = true;
  bool     fs_font = false;    // For ESP32/8266 use smooth font file or FLASH (PROGMEM) array

#else
  bool     fontFile = true;
#endif

  private:

  uint32_t readInt32(void);

  uint8_t* fontPtr = nullptr;

