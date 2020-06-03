#ifndef FREETYPE_SCENE_HPP
#define FREETYPE_SCENE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "scene.hpp"
#include <string>
#include <vector>

class FreeTypeScene : public Scene {
public:
  virtual bool Init(SDL_Renderer *renderer);
  virtual void Tick(SDL_Renderer *renderer);
  virtual void Cleanup(SDL_Renderer *renderer);

private:
  static void DrawText(const std::wstring &text, const SDL_Color &color,
                       const int &baseline, const int &x_start,
                       const FT_Face &face, SDL_Renderer *renderer);

  std::string TEXT = "Test";
  const char *FONT = "Sarabun-Regular.ttf";
  FT_Face face;
  int fontSize = 64;
  SDL_Color color;

  std::vector<char> buffer;
  static constexpr size_t bufferSize = 256;
};

#endif
