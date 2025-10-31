#include "render_sdl.h"

#include <cstdio>
#include <cstdlib>

SDLRenderer::SDLRenderer(int width, int height, int scale_factor)
    : window(nullptr), renderer(nullptr), texture(nullptr),
      framebuffer(nullptr), fb_width(width), fb_height(height),
      scale(scale_factor) {}

SDLRenderer::~SDLRenderer() { cleanup(); }

bool SDLRenderer::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL initialization failed: %s\n", SDL_GetError());
    return false;
  }

  window = SDL_CreateWindow("TFT_eSPI Simulator", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, fb_width * scale,
                            fb_height * scale, SDL_WINDOW_SHOWN);

  if (!window) {
    printf("Window creation failed: %s\n", SDL_GetError());
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("Renderer creation failed: %s\n", SDL_GetError());
    return false;
  }

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565,
                              SDL_TEXTUREACCESS_STREAMING, fb_width, fb_height);

  if (!texture) {
    printf("Texture creation failed: %s\n", SDL_GetError());
    return false;
  }

  // Allocate framebuffer
  framebuffer = (uint16_t *)calloc(fb_width * fb_height, sizeof(uint16_t));

  printf("SDL init: %dx%d window (scale=%d)\n", fb_width, fb_height, scale);
  return true;
}

// Update SDL display with sprite content
void SDLRenderer::updateDisplay(TFT_eSprite *sprite, int x, int y) {
  if (!framebuffer || !sprite)
    return;

  // Get sprite dimensions
  int16_t w = sprite->width();
  int16_t h = sprite->height();

  // Get sprite buffer (assuming 16-bit color depth)
  auto *spriteBuffer = (uint16_t *)sprite->getPointer();

  if (!spriteBuffer)
    return;

  // Copy sprite to framebuffer at position (x, y)
  for (int16_t sy = 0; sy < h; sy++) {
    for (int16_t sx = 0; sx < w; sx++) {
      int fx = x + sx;
      int fy = y + sy;

      if (fx >= 0 && fx < fb_width && fy >= 0 && fy < fb_height) {
        uint16_t color = sprite->readPixel(sx, sy);
        framebuffer[fy * fb_width + fx] = color;
      }
    }
  }

  // Update texture
  SDL_UpdateTexture(texture, nullptr, framebuffer, fb_width * sizeof(uint16_t));

  // Render to screen
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);
}

// Run event loop with optional update callback
void SDLRenderer::runEventLoop(const std::function<void()> &updateCallback) {
  printf("\nPress Ctrl + C or close window to exit...\n");
  bool running = true;
  SDL_Event event;

  while (running) {
    // Process events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
      }
    }

    // Call update callback if provided
    if (updateCallback) {
      updateCallback();
    }

    // Maintain ~60 FPS
    SDL_Delay(16);
  }
}

// Cleanup SDL resources
void SDLRenderer::cleanup() {
  if (framebuffer) {
    free(framebuffer);
    framebuffer = nullptr;
  }
  if (texture) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  SDL_Quit();
}
