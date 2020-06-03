#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include<SDL2/SDL.h>

#include <ft2build.h>
#include FT_FREETYPE_H

struct Context {
  SDL_Renderer *renderer;
  FT_Library ftLibrary;
};

#endif