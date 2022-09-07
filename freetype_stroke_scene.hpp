#ifndef FREETYPE_STROKE_SCENE_HPP
#define FREETYPE_STROKE_SCENE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include <array>
#include <string>

#include "scene.hpp"

class FreeTypeStrokeScene : public Scene {
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
                       const FT_Stroker &stroker, const SDL_Color &border_color,
                       SDL_Renderer *renderer);

  static void DrawGlyph(FT_Glyph glyph, const SDL_Color &color, int &x,
                        const int &baseline, SDL_Renderer *renderer);

  FT_Face face;
  FT_Stroker stroker;

  int fontSize = 64;
  SDL_Color color = {0, 0, 0, 255};

  int borderSize = 2;
  SDL_Color border_color = {0xEE, 0x10, 0xCC};
  std::array<char, BufferSize> buffer {"Hello World"};
};

#endif
