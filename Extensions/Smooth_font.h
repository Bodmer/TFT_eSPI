 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the TFT_eSPI class and is associated with anti-aliased font function
protected:
  TFT_eSPI_SmoothFont * sf = nullptr;
  bool     fontLoaded = false; // Flags when a anti-aliased font is loaded

 public:
  // These are for the new antialiased fonts
  void     loadFont(const uint8_t array[]);
#ifdef FONT_FS_AVAILABLE
  void     loadFont(String fontName, fs::FS &ffs);
#endif
  void     loadFont(String fontName, bool flash = true);

  void     setFont( TFT_eSPI_SmoothFont * font) { sf = font; fontLoaded = true;};
  void     unloadFont( void );

  virtual void drawGlyph(uint16_t code);

  void     showFont(uint32_t td);




