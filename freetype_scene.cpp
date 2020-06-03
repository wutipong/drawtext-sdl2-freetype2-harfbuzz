#include "freetype_scene.hpp"

#include <algorithm>
#include <codecvt>
#include <imgui.h>

#include "menu_scene.hpp"
#include "texture.hpp"
#include <utf8.h>

static FT_Library library;

bool FreeTypeScene::Init(SDL_Renderer *renderer) {
  FT_Init_FreeType(&library);
  auto error = FT_New_Face(library, FONT, 0, &face);
  if (error)
    return false;

  std::fill(std::begin(buffer), std::end(buffer), 0);
  std::copy(TEXT.begin(), TEXT.end(), std::begin(buffer));

  return true;
}

void FreeTypeScene::Tick(SDL_Renderer *renderer) {
  ImGui::Begin("Menu");
  if (ImGui::Button("Back")) {
    ImGui::End();
    ChangeScene<MenuScene>(renderer);

    return;
  }

  ImGui::InputText("text", buffer, bufferSize);

  ImGui::SliderInt("font size", &fontSize, 0, 128);
  ImGui::End();

  SDL_Color color;
  color.r = 0x80;
  color.g = 0xff;
  color.b = 0xaa;

  FT_Set_Pixel_Sizes(face, 0, fontSize);

  std::wstring text;
  utf8::utf8to16(std::begin(buffer), std::end(buffer),
                 std::back_inserter(text));

  DrawText(text, color, 300, 300, face, renderer);
}

void FreeTypeScene::Cleanup(SDL_Renderer *renderer) { FT_Done_Face(face); }

void FreeTypeScene::DrawText(const std::wstring &text, const SDL_Color &color,
                             const int &baseline, const int &x_start,
                             const FT_Face &face, SDL_Renderer *renderer) {
  int x = x_start;

  for (auto c : text) {
    FT_Load_Char(face, c, FT_LOAD_RENDER);

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
