 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the TFT_eSPI class and is associated with anti-aliased font functions

 public:

  // These are for the new antialiased fonts
  void     loadFont(String fontName);
  void     unloadFont( void );
  bool     getUnicodeIndex(uint16_t unicode, uint16_t *index);

  uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);
  uint16_t decodeUTF8(uint8_t c);

  uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);

  virtual void drawGlyph(uint16_t code);

  void     showFont(uint32_t td);

 fs::File   fontFile;

  // This is for the whole font
  typedef struct
  {
    uint16_t gCount;     // Total number of characters
    uint16_t yAdvance;   // Line advance
    uint16_t spaceWidth; // Width of a space character
    int16_t  ascent;     // Height of top of 'd' above baseline, other characters may be taller
    int16_t  descent;    // Offset to bottom of 'p', other characters may have a larger descent
    uint16_t maxAscent;  // Maximum ascent found in font
    uint16_t maxDescent; // Maximum descent found in font
  } fontMetrics;

fontMetrics gFont = { 0, 0, 0, 0, 0, 0, 0 };

  // These are for the metrics for each individual glyph (so we don't need to seek this in file and waste time)
  uint16_t* gUnicode = NULL;  //UTF-16 code, the codes are searched so do not need to be sequential
  uint8_t*  gHeight = NULL;   //cheight
  uint8_t*  gWidth = NULL;    //cwidth
  uint8_t*  gxAdvance = NULL; //setWidth
  int8_t*   gdY = NULL;       //topExtent
  int8_t*   gdX = NULL;       //leftExtent
  uint32_t* gBitmap = NULL;   //file pointer to greyscale bitmap

  String   _gFontFilename;

  uint8_t  decoderState = 0;   // UTF8 decoder state
  uint16_t decoderBuffer;      // Unicode code-point buffer

  bool     fontLoaded = false; // Flags when a anti-aliased font is loaded

 private:

  void     loadMetrics(uint16_t gCount);
  uint32_t readInt32(void);
