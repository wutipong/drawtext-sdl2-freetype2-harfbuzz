#include "freetype_scene.hpp"

#include <imgui.h>

#include "menu_scene.hpp"
#include "texture.hpp"

static FT_Library library;

bool FreeTypeScene::Init(SDL_Renderer *renderer) {
  FT_Init_FreeType(&library);
  auto error = FT_New_Face(library, FONT, 0, &face);
  if (error)
    return false;

  return true;
}

void FreeTypeScene::Tick(SDL_Renderer *renderer) {
  ImGui::Begin("Menu");
  if (ImGui::Button("Back")) {
    ImGui::End();
    ChangeScene<MenuScene>(renderer);

    return;
  }

  ImGui::SliderInt("font size", &fontSize, 0, 128);
  ImGui::End();

  SDL_Color color;
  color.r = 0x80;
  color.g = 0xff;
  color.b = 0xaa;

  FT_Set_Pixel_Sizes(face, 0, fontSize);
  DrawText(TEXT, color, 300, 300, face, renderer);
}

void FreeTypeScene::Cleanup(SDL_Renderer *renderer) { FT_Done_Face(face); }

void FreeTypeScene::DrawText(const std::wstring &text, const SDL_Color &color,
                             const int &baseline, const int &x_start,
                             const FT_Face &face, SDL_Renderer *renderer) {
  int x = x_start;

  for (unsigned int i = 0; i < text.length(); i++) {
    FT_Load_Char(face, text[i], FT_LOAD_RENDER);

    SDL_Texture *glyph_texture =
        CreateTextureFromFT_Bitmap(renderer, face->glyph->bitmap, color);

    if (glyph_texture != nullptr) {
      SDL_Rect dest;
      SDL_QueryTexture(glyph_texture, NULL, NULL, &dest.w, &dest.h);
      dest.x = x + (face->glyph->metrics.horiBearingX >> 6);
      dest.y = baseline - (face->glyph->metrics.horiBearingY >> 6);

      SDL_SetTextureBlendMode(glyph_texture, SDL_BLENDMODE_BLEND);
      SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);
      SDL_DestroyTexture(glyph_texture);
    }

    x += (face->glyph->metrics.horiAdvance >> 6);
  }
}