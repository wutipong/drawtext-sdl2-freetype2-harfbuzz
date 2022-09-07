#ifndef FREETYPE_HARFBUZZ_SCENE_HPP
#define FREETYPE_HARFBUZZ_SCENE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <array>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <string>

#include "scene.hpp"

class FreeTypeHarfbuzzScene : public Scene {
public:
  bool Init(const Context &context) override;
  void Tick(const Context &context) override;
  void Cleanup(const Context &context) override;

private:
  static constexpr size_t BufferSize{256};
  static constexpr auto FontFile{"Sarabun-Regular.ttf"};

  static void DrawText(const std::array<char, BufferSize> &text,
                       const SDL_Color &color, const int &baseline,
                       const int &x_start, const FT_Face &face,
                       hb_font_t *hb_font, SDL_Renderer *renderer);

  int fontSize = 64;
  SDL_Color color = {0, 0, 0, 255};

  FT_Face face;
  hb_font_t *hb_font;

  std::array<char, BufferSize> buffer{"Hello World"};
};

#endif
