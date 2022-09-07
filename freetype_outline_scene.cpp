#include "freetype_outline_scene.hpp"

#include <algorithm>
#include <array>
#include <imgui.h>

#include "menu_scene.hpp"
#include "texture.hpp"
#include <utf8cpp/utf8.h>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

bool FreeTypeOutlineScene::Init(const Context &context) {
  auto error = FT_New_Face(context.ftLibrary, FontFile, 0, &face);
  if (error)
    return false;

  return true;
}

void FreeTypeOutlineScene::Tick(const Context &context) {

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

  DrawText(buffer, color, 300, 300, face, context);
}

void FreeTypeOutlineScene::Cleanup(const Context &context) {
  FT_Done_Face(face);
}

void FreeTypeOutlineScene::DrawSpansCallback(const int y, const int count,
                                             const FT_Span *const spans,
                                             void *const user) {
  auto *pData = static_cast<SpanAdditionData *>(user);
  for (int i = 0; i < count; ++i) {
    int x1 = spans[i].x + pData->dest.x;
    int y1 = pData->dest.y - y;
    int x2 = x1 + spans[i].len;
    int y2 = y1;

    SDL_SetRenderDrawBlendMode(pData->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(pData->renderer, pData->color.r, pData->color.g,
                           pData->color.b, spans[i].coverage);
    SDL_RenderDrawLine(pData->renderer, x1, y1, x2, y2);
  }
}

void FreeTypeOutlineScene::DrawText(const std::array<char, BufferSize> &text,
                                    const SDL_Color &color, const int &baseline,
                                    const int &x_start, const FT_Face &face,
                                    const Context &context) {

  std::vector<FT_ULong> characters;
  auto end_it = utf8::find_invalid(text.begin(), text.end());
  utf8::utf8to16(text.begin(), end_it, std::back_inserter(characters));

  int x = x_start;

  SpanAdditionData additionData{};
  additionData.color = color;

  for (auto c : characters) {
    FT_Load_Char(face, c, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING);
    additionData.dest.x = x;
    additionData.dest.y = baseline;
    additionData.renderer = context.renderer;

    if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
      FT_Raster_Params params;
      memset(&params, 0, sizeof(params));
      params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
      params.gray_spans = DrawSpansCallback;
      params.user = &additionData;

      FT_Outline_Render(context.ftLibrary, &face->glyph->outline, &params);
    } // No fallback
    x += (face->glyph->metrics.horiAdvance >> 6);
  }
}
