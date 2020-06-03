#include "freetype_scene.hpp"

#include <algorithm>
#include <codecvt>
#include <imgui.h>

#include "menu_scene.hpp"
#include "texture.hpp"
#include <utf8.h>

bool FreeTypeScene::Init(const Context &context) {
  auto error = FT_New_Face(context.ftLibrary, FONT, 0, &face);
  if (error)
    return false;

  buffer.resize(bufferSize);
  std::copy(TEXT.begin(), TEXT.end(), buffer.begin());

  return true;
}

void FreeTypeScene::Tick(const Context &context) {
  ImGui::Begin("Menu");
  ImGui::InputText("text", buffer.data(), bufferSize);
  ImGui::SliderInt("font size", &fontSize, 0, 128);

  float c[4]{color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0};

  ImGui::ColorPicker4("color", c, ImGuiColorEditFlags_InputRGB);
  color.r = c[0] * 255;
  color.g = c[1] * 255;
  color.b = c[2] * 255;

  bool quit = ImGui::Button("Back");

  ImGui::End();

  if (quit) {
    ChangeScene<MenuScene>(context);

    return;
  }

  FT_Set_Pixel_Sizes(face, 0, fontSize);

  std::wstring text;
  utf8::utf8to16(buffer.begin(), buffer.end(), std::back_inserter(text));

  DrawText(text, color, 300, 300, face, context.renderer);
}

void FreeTypeScene::Cleanup(const Context &context) { FT_Done_Face(face); }

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
