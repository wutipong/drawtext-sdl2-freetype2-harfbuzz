#ifndef FREETYPE_STROKE_SCENE_HPP
#define FREETYPE_STROKE_SCENE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include <string>

#include "scene.hpp"

class FreeTypeStrokeScene : public Scene {
public:
  virtual bool Init(SDL_Renderer *renderer);
  virtual void Tick(SDL_Renderer *renderer);
  virtual void Cleanup(SDL_Renderer *renderer);

private:
  static void DrawText(const std::wstring &text, const SDL_Color &color,
                       const int &baseline, const int &x_start,
                       const FT_Face &face, const FT_Stroker &stroker,
                       const SDL_Color &border_color, SDL_Renderer *&renderer);

  static void FreeTypeStrokeScene::DrawGlyph(FT_Glyph glyph,
                                             const SDL_Color &color, int &x,
                                             const int &baseline,
                                             SDL_Renderer *renderer);

  const std::wstring TEXT = L"เก็บใจ เก็บไว้มานาน เก็บมันคล้าย ๆ รอใคร";
  const char *FONT = "Sarabun-Regular.ttf";
  FT_Face face;
  FT_Stroker stroker;
};

#endif