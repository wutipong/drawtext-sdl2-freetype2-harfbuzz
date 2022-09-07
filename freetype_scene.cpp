#include "freetype_scene.hpp"

#include <algorithm>
#include <vector>

#include <imgui.h>
#include <utf8.h>

#include "menu_scene.hpp"
#include "texture.hpp"

bool FreeTypeScene::Init(const Context &context) {
  auto error = FT_New_Face(context.ftLibrary, FontFile, 0, &face);
  if (error)
    return false;

  return true;
}

void FreeTypeScene::Tick(const Context &context) {

  ImGui::Begin("Menu");
  ImGui::InputText("text", buffer.data(), buffer.size());

  ImGui::SliderInt("font size", &fontSize, 0, 128);

  float c[]{static_cast<float>(color.r) / 255.0f,
            static_cast<float>(color.g) / 255.0f,
            static_cast<float>(color.b) / 255.0f};

  ImGui::ColorPicker4("color", c, ImGuiColorEditFlags_InputRGB);
  color.r = static_cast<Uint8>(c[0] * 255);
  color.g = static_cast<Uint8>(c[1] * 255);
  color.b = static_cast<Uint8>(c[2] * 255);

  bool quit = ImGui::Button("Back");

  ImGui::End();

  if (quit) {
    ChangeScene<MenuScene>(context);

    return;
  }

  FT_Set_Pixel_Sizes(face, 0, fontSize);

  DrawText(buffer, color, 300, 300, face, context.renderer);
}

void FreeTypeScene::Cleanup(const Context &context) { FT_Done_Face(face); }

void FreeTypeScene::DrawText(const std::array<char, BufferSize> &text,
                             const SDL_Color &color, const int &baseline,
                             const int &x_start, const FT_Face &face,
                             SDL_Renderer *renderer) {
  int x = x_start;

  std::vector<wchar_t> charactors;
  auto end_it = std::find(text.begin(), text.end(), 0);
  end_it = utf8::find_invalid(text.begin(), end_it);

  utf8::utf8to16(text.begin(), end_it, std::back_inserter(charactors));

  for (auto c : charactors) {
    FT_Load_Char(face, c, FT_LOAD_RENDER);

    SDL_Texture *glyph_texture =
        CreateTextureFromFT_Bitmap(renderer, face->glyph->bitmap);

    if (glyph_texture != nullptr) {
      SDL_Rect dest;
      SDL_QueryTexture(glyph_texture, nullptr, nullptr, &dest.w, &dest.h);
      dest.x = x + (face->glyph->metrics.horiBearingX >> 6);
      dest.y = baseline - (face->glyph->metrics.horiBearingY >> 6);

      SDL_SetTextureBlendMode(glyph_texture, SDL_BLENDMODE_BLEND);
      SDL_SetTextureColorMod(glyph_texture, color.r, color.g, color.b);
      SDL_RenderCopy(renderer, glyph_texture, nullptr, &dest);
      SDL_DestroyTexture(glyph_texture);
    }

    x += (face->glyph->metrics.horiAdvance >> 6);
  }
}
