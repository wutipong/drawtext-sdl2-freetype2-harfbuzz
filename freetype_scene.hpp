#ifndef FREETYPE_SCENE_HPP
#define FREETYPE_SCENE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "scene.hpp"
#include <array>
#include <string>

class FreeTypeScene : public Scene {
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
                       SDL_Renderer *renderer);

  FT_Face face;
  int fontSize = 64;
  SDL_Color color = {0, 0, 0, 255};

  std::array<char, BufferSize> buffer{"Hello World"};
};

#endif
