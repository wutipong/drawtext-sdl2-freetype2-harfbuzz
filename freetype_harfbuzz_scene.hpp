#ifndef FREETYPE_HARFBUZZ_SCENE_HPP
#define FREETYPE_HARFBUZZ_SCENE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <string>
#include <vector>

#include "scene.hpp"

class FreeTypeHarfbuzzScene : public Scene {
public:
  virtual bool Init(SDL_Renderer *renderer);
  virtual void Tick(SDL_Renderer *renderer);
  virtual void Cleanup(SDL_Renderer *renderer);

private:
  static void DrawText(const std::wstring &text, const SDL_Color &color,
                       const int &baseline, const int &x_start,
                       const FT_Face &face, hb_font_t *hb_font,
                       SDL_Renderer *renderer);

  std::string TEXT = "Test";
  const char *FONT = "Sarabun-Regular.ttf";
  int fontSize = 64;
  SDL_Color color;

  FT_Face face;
  hb_font_t *hb_font;

  std::vector<char> buffer;
  static constexpr size_t bufferSize = 256;
};

#endif
