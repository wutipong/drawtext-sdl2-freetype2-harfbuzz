#ifndef FREETYPE_HARFBUZZ_SCENE_HPP
#define FREETYPE_HARFBUZZ_SCENE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>

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

  const std::wstring TEXT = L"เก็บใจ เก็บไว้มานาน เก็บมันคล้าย ๆ รอใคร";
  const char *FONT = "Sarabun-Regular.ttf";

  FT_Face face;
  hb_font_t *hb_font;
};

#endif