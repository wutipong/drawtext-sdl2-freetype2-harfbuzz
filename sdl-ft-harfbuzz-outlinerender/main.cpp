#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include <string>

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>

const std::wstring TEXT = L"ค่ำคืน มึดมิด กี่โมง ไม่รู้ 555 มึนซะแล้วสินะ";
const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

SDL_Renderer *renderer;
FT_Library library;

struct SpanAdditionData {
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

    SDL_SetRenderDrawColor(renderer, addl->color.r, addl->color.g,
                           addl->color.b, spans[i].coverage);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
  }
}

void DrawText(const std::wstring &text, const SDL_Color &color,
              const int &baseline, const int &x_start, const FT_Face &face,
              hb_font_t *hb_font, SDL_Renderer *&renderer) {

  hb_buffer_t *buffer = hb_buffer_create();

  hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
  hb_buffer_set_script(buffer, HB_SCRIPT_THAI);

  hb_buffer_add_utf16(buffer, (unsigned short *)(text.c_str()), text.length(),
                      0, text.length());

  hb_shape(hb_font, buffer, NULL, 0);

  const unsigned int glyph_count = hb_buffer_get_length(buffer);
  const hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
  const hb_glyph_position_t *glyph_positions =
      hb_buffer_get_glyph_positions(buffer, NULL);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  int x = x_start;

  SpanAdditionData addl;

  addl.color = color;

  for (unsigned int i = 0; i < glyph_count; i++) {
    FT_Load_Glyph(face, glyph_infos[i].codepoint, FT_LOAD_NO_BITMAP);

    addl.dest.x = x + (glyph_positions[i].x_offset >> 6);
    addl.dest.y = baseline - (glyph_positions[i].y_offset >> 6);

    if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
      FT_Raster_Params params;
      memset(&params, 0, sizeof(params));
      params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
      params.gray_spans = DrawSpansCallback;
      params.user = &addl;

      FT_Outline_Render(library, &face->glyph->outline, &params);
    } // No fallback
    x += (glyph_positions[i].x_advance >> 6);
  }

  hb_buffer_destroy(buffer);
}

int main(int argc, char **argv) {
  if (argc != 2)
    return -1;

  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *window =
      SDL_CreateWindow("Test Window", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);

  renderer = SDL_CreateRenderer(window, -1, 0);

  FT_Init_FreeType(&library);

  FT_Face face;
  FT_New_Face(library, argv[1], 0, &face);
  FT_Set_Pixel_Sizes(face, 0, 64);

  hb_font_t *hb_font = hb_ft_font_create(face, 0);

  SDL_Color color;
  color.a = 0xff;
  color.r = 0xff;
  color.g = 0xa0;
  color.b = 0x80;

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT)
        break;
    }

    SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
    SDL_RenderClear(renderer);

    DrawText(TEXT, color, 300, 30, face, hb_font, renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }

  hb_font_destroy(hb_font);
  FT_Done_Face(face);
  FT_Done_FreeType(library);

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
