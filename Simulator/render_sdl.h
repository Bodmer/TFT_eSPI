#pragma once

#include "TFT_eSPI.h"

#include <SDL.h>
#include <functional>

class SDLRenderer {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  uint16_t *framebuffer;
  int fb_width;
  int fb_height;
  int scale;

public:
  explicit SDLRenderer(int width = 240, int height = 320, int scale_factor = 2);

  ~SDLRenderer();

  bool init();

  // Update SDL display with sprite content
  void updateDisplay(TFT_eSprite *sprite, int x, int y);

  // Run event loop with optional update callback
  static void
  runEventLoop(const std::function<void()> &updateCallback = nullptr);

  // Cleanup SDL resources
  void cleanup();

  // Getters
  int getWidth() const { return fb_width; }
  int getHeight() const { return fb_height; }
  int getScale() const { return scale; }
};
