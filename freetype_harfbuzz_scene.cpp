#include "freetype_harfbuzz_scene.hpp"

#include <vector>

#include <imgui.h>
#include <utf8.h>

#include "menu_scene.hpp"
#include "texture.hpp"

bool FreeTypeHarfbuzzScene::Init(const Context &context) {
  auto error = FT_New_Face(context.ftLibrary, FontFile, 0, &face);
  if (error)
    return false;

  FT_Set_Pixel_Sizes(face, 0, 64);
  hb_font = hb_ft_font_create(face, nullptr);

  return true;
}

void FreeTypeHarfbuzzScene::Tick(const Context &context) {
  ImGui::Begin("Menu");
  ImGui::InputText("text", buffer.data(), BufferSize);
  ImGui::SliderInt("font size", &fontSize, 0, 128);

  float c[]{static_cast<float>(color.r) / 255.0f,
             static_cast<float>(color.g) / 255.0f,
             static_cast<float>(color.b) / 255.0f};

  ImGui::ColorPicker3("color", c, ImGuiColorEditFlags_InputRGB);
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
  hb_ft_font_changed(hb_font);

  DrawText(buffer, color, 300, 300, face, hb_font, context.renderer);
}

void FreeTypeHarfbuzzScene::Cleanup(const Context &context) {
  hb_font_destroy(hb_font);
  FT_Done_Face(face);
}

void FreeTypeHarfbuzzScene::DrawText(const std::array<char, BufferSize> &text,
                                     const SDL_Color &color,
                                     const int &baseline, const int &x_start,
                                     const FT_Face &face, hb_font_t *hb_font,
                                     SDL_Renderer *renderer) {
  hb_buffer_t *buffer = hb_buffer_create();

  hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
  hb_buffer_set_script(buffer, HB_SCRIPT_THAI);

  std::vector<uint16_t> characters;
  auto end_it = std::find(text.begin(), text.end(), 0);
  end_it = utf8::find_invalid(text.begin(), end_it);

  utf8::utf8to16(text.begin(), end_it, std::back_inserter(characters));

  hb_buffer_add_utf16(buffer, characters.data(),
                      static_cast<int>(characters.size()), 0,
                      static_cast<int>(characters.size()));

  hb_shape(hb_font, buffer, nullptr, 0);

  unsigned int glyph_count = hb_buffer_get_length(buffer);
  hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, nullptr);
  hb_glyph_position_t *glyph_positions =
      hb_buffer_get_glyph_positions(buffer, nullptr);

  int x = x_start;

  for (unsigned int i = 0; i < glyph_count; i++) {
    FT_Load_Glyph(face, glyph_infos[i].codepoint, FT_LOAD_RENDER);

    SDL_Texture *glyph_texture =
        CreateTextureFromFT_Bitmap(renderer, face->glyph->bitmap);

    if (glyph_texture != nullptr) {
      SDL_Rect dest;
      SDL_QueryTexture(glyph_texture, nullptr, nullptr, &dest.w, &dest.h);

      // FreeType's metrics is in 26.6 fixed-point format.
      // shift the number to the right by 6 bit to round it off to integer.
      dest.x = x + (face->glyph->metrics.horiBearingX >> 6) +
               (glyph_positions[i].x_offset >> 6);
      dest.y = baseline - (face->glyph->metrics.horiBearingY >> 6) -
               (glyph_positions[i].y_offset >> 6);

      SDL_SetTextureBlendMode(glyph_texture, SDL_BLENDMODE_BLEND);
      SDL_SetTextureColorMod(glyph_texture, color.r, color.g, color.b);
      SDL_RenderCopy(renderer, glyph_texture, nullptr, &dest);
      SDL_DestroyTexture(glyph_texture);
    }

    x += (glyph_positions[i].x_advance >> 6);
  }

  hb_buffer_destroy(buffer);
}
