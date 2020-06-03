#include "freetype_harfbuzz_scene.hpp"

#include <imgui.h>

#include "menu_scene.hpp"
#include "texture.hpp"

static FT_Library library;

bool FreeTypeHarfbuzzScene::Init(SDL_Renderer *renderer) {
  FT_Init_FreeType(&library);
  auto error = FT_New_Face(library, FONT, 0, &face);
  if (error)
    return false;

  FT_Set_Pixel_Sizes(face, 0, 64);
  hb_font = hb_ft_font_create(face, 0);

  return true;
}

void FreeTypeHarfbuzzScene::Tick(SDL_Renderer *renderer) {
  ImGui::Begin("Menu");
  if (ImGui::Button("Back")) {
    ImGui::End();
    ChangeScene<MenuScene>(renderer);

    return;
  }
  ImGui::End();

  SDL_Color color;
  color.r = 0x80;
  color.g = 0xff;
  color.b = 0xaa;

  DrawText(TEXT, color, 300, 300, face, hb_font, renderer);
}

void FreeTypeHarfbuzzScene::Cleanup(SDL_Renderer *renderer) {
  hb_font_destroy(hb_font);
  FT_Done_Face(face);
}

void FreeTypeHarfbuzzScene::DrawText(const std::wstring &text,
                                     const SDL_Color &color,
                                     const int &baseline, const int &x_start,
                                     const FT_Face &face, hb_font_t *hb_font,
                                     SDL_Renderer *renderer) {
  hb_buffer_t *buffer = hb_buffer_create();

  hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
  hb_buffer_set_script(buffer, HB_SCRIPT_THAI);

  hb_buffer_add_utf16(buffer, (unsigned short *)(text.c_str()), text.length(),
                      0, text.length());

  hb_shape(hb_font, buffer, NULL, 0);

  unsigned int glyph_count = hb_buffer_get_length(buffer);
  hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
  hb_glyph_position_t *glyph_positions =
      hb_buffer_get_glyph_positions(buffer, NULL);

  int x = x_start;

  for (unsigned int i = 0; i < glyph_count; i++) {
    FT_Load_Glyph(face, glyph_infos[i].codepoint, FT_LOAD_RENDER);

    SDL_Texture *glyph_texture =
        CreateTextureFromFT_Bitmap(renderer, face->glyph->bitmap, color);

    if (glyph_texture != nullptr) {
      SDL_Rect dest;
      SDL_QueryTexture(glyph_texture, NULL, NULL, &dest.w, &dest.h);

      dest.x = x + (face->glyph->metrics.horiBearingX >> 6) +
               (glyph_positions[i].x_offset >> 6);
      dest.y = baseline - (face->glyph->metrics.horiBearingY >> 6) -
               (glyph_positions[i].y_offset >> 6);

      SDL_SetTextureBlendMode(glyph_texture, SDL_BLENDMODE_BLEND);
      SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);
      SDL_DestroyTexture(glyph_texture);
    }

    x += (glyph_positions[i].x_advance >> 6);
  }

  hb_buffer_destroy(buffer);
}