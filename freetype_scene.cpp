#include "freetype_scene.hpp"

#include <imgui.h>

#include "menu_scene.hpp"

static FT_Library library;

bool FreeTypeScene::Init(SDL_Renderer *renderer) {
  FT_Init_FreeType(&library);
  auto error = FT_New_Face(library, FONT, 0, &face);
  if (error)
    return false;

  FT_Set_Pixel_Sizes(face, 0, 64);

  return true;
}

void FreeTypeScene::Tick(SDL_Renderer *renderer) {
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

  DrawText(TEXT, color, 300, 300, face, renderer);
}

void FreeTypeScene::Cleanup(SDL_Renderer *renderer) { FT_Done_Face(face); }

SDL_Texture *FreeTypeScene::CreateTextureFromFT_Bitmap(SDL_Renderer *renderer,
                                                       const FT_Bitmap &bitmap,
                                                       const SDL_Color &color) {
  SDL_Texture *output =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, bitmap.width, bitmap.rows);

  if (bitmap.width == 0 || bitmap.rows == 0) {
    return nullptr;
  }
  void *buffer;
  int pitch;
  SDL_LockTexture(output, NULL, &buffer, &pitch);

  unsigned char *src_pixels = bitmap.buffer;
  unsigned int *target_pixels = reinterpret_cast<unsigned int *>(buffer);

  SDL_PixelFormat *pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

  for (int y = 0; y < bitmap.rows; y++) {
    for (int x = 0; x < bitmap.width; x++) {
      int index = (y * bitmap.width) + x;

      unsigned int alpha = src_pixels[index];
      unsigned int pixel_value =
          SDL_MapRGBA(pixel_format, color.r, color.g, color.b, alpha);

      target_pixels[index] = pixel_value;
    }
  }

  SDL_FreeFormat(pixel_format);
  SDL_UnlockTexture(output);

  return output;
}

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
