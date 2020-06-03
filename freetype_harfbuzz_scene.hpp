#ifndef FREETYPE_HARFBUZZ_SCENE_HPP
#define FREETYPE_HARFBUZZ_SCENE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <string>
#include <array>

#include "scene.hpp"

class FreeTypeHarfbuzzScene : public Scene {
public:
  virtual bool Init(const Context &context);
  virtual void Tick(const Context &context);
  virtual void Cleanup(const Context &context);

private:
  static constexpr size_t bufferSize = 256;
  static void DrawText(const std::array<char, bufferSize> &text,
                       const SDL_Color &color, const int &baseline,
                       const int &x_start, const FT_Face &face,
                       hb_font_t *hb_font, SDL_Renderer *renderer);

  const std::string TEXT = "Test";
  const char *FONT = "Sarabun-Regular.ttf";

  int fontSize = 64;
  SDL_Color color = {0, 0, 0, 255};

  FT_Face face;
  hb_font_t *hb_font;

  std::array<char, bufferSize> buffer;
};

#endif
