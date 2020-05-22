#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

const std::wstring text = L"ชนใดไม่มีดนตรีกาลในสันดานเป็นคนชอบกลนัก";
const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

struct SpanAdditionData {
  SDL_Renderer *renderer;
  SDL_Point dest;
  SDL_Color color;
};

void DrawSpansCallback(const int y, const int count, const FT_Span *const spans,
                       void *const user) {
  SpanAdditionData *addl = static_cast<SpanAdditionData *>(user);
  for (int i = 0; i < count; ++i) {
    int x1 = spans[i].x + addl->dest.x;
    int y1 = addl->dest.y - y;
    int x2 = x1 + spans[i].len - 1;
    int y2 = y1;

    SDL_SetRenderDrawColor(addl->renderer, addl->color.r, addl->color.g,
                           addl->color.b, spans[i].coverage);
    SDL_RenderDrawLine(addl->renderer, x1, y1, x2, y2);
  }
}

void DrawText(const std::wstring &text, const SDL_Color &color,
              const int &baseline, const int &x_start,
              const FT_Library &library, const FT_Face &face,
              SDL_Renderer *renderer) {

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  int x = x_start;

  SpanAdditionData addl;
  addl.color = color;

  for (unsigned int i = 0; i < text.length(); i++) {
    FT_Load_Char(face, text[i], FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
    addl.dest.x = x;
    addl.dest.y = baseline;
    addl.renderer = renderer;

    if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
      FT_Raster_Params params;
      memset(&params, 0, sizeof(params));
      params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
      params.gray_spans = DrawSpansCallback;
      params.user = &addl;

      FT_Outline_Render(library, &face->glyph->outline, &params);
    } // No fallback
    x += (face->glyph->metrics.horiAdvance >> 6);
  }
}

int main(int argc, char **argv) {
  if (argc != 2)
    return -1;

  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *window =
      SDL_CreateWindow("Test Window", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
  FT_Library library;
  FT_Init_FreeType(&library);

  FT_Face face;
  FT_New_Face(library, argv[1], 0, &face);
  FT_Set_Pixel_Sizes(face, 0, 56);

  SDL_Texture *renderTarget =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                        SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        break;
    }

    SDL_SetRenderTarget(renderer, renderTarget);
    SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
    SDL_RenderClear(renderer);

    SDL_Color color;
    color.r = 0xff;
    color.g = 0x80;
    color.b = 0xb0;
    color.a = 0xff;

    DrawText(text, color, 300, 120, library, face, renderer);

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, renderTarget, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Delay(10);
  }

  SDL_DestroyTexture(renderTarget);

  FT_Done_Face(face);
  FT_Done_FreeType(library);

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}