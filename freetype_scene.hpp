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
  virtual bool Init(const Context &context);
  virtual void Tick(const Context &context);
  virtual void Cleanup(const Context &context);

private:
  static constexpr size_t bufferSize = 256;
  static void DrawText(const std::array<char, bufferSize> &text,
                       const SDL_Color &color, const int &baseline,
                       const int &x_start, const FT_Face &face,
                       SDL_Renderer *renderer);

  std::string TEXT = "Test";
  const char *FONT = "Sarabun-Regular.ttf";
  FT_Face face;
  int fontSize = 64;
  SDL_Color color = {0, 0, 0, 255};

  std::array<char, bufferSize> buffer;
};

#endif
