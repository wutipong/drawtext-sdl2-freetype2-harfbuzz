#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

SDL_Texture *CreateTextureFromFT_Bitmap(SDL_Renderer *renderer,
                                        FT_Bitmap &bitmap);

#endif