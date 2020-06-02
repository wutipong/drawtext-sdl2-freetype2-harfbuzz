#include "freetype_stroke_scene.hpp"

#include <imgui.h>

#include "menu_scene.hpp"

static FT_Library library;

bool FreeTypeStrokeScene::Init(SDL_Renderer* renderer) {
    FT_Init_FreeType(&library);
    auto error = FT_New_Face(library, FONT, 0, &face);
    if (error)
        return false;

    FT_Set_Pixel_Sizes(face, 0, 64);


    FT_Stroker_New(library, &stroker);
    FT_Stroker_Set(stroker, 2 << 6, FT_STROKER_LINECAP_ROUND,
        FT_STROKER_LINEJOIN_ROUND, 0);

    return true;
}

void FreeTypeStrokeScene::Tick(SDL_Renderer* renderer) {
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

    SDL_Color border_color;
    border_color.r = 0xEE;
    border_color.g = 0x10;
    border_color.b = 0xCC;

    DrawText(TEXT, color, 300, 300, face, stroker, border_color, renderer);
}

void FreeTypeStrokeScene::Cleanup(SDL_Renderer* renderer) {
    FT_Stroker_Done(stroker);
    FT_Done_Face(face); 
}

SDL_Texture* FreeTypeStrokeScene::CreateTextureFromFT_Bitmap(SDL_Renderer* renderer,
    const FT_Bitmap& bitmap,
    const SDL_Color& color) {
    SDL_Texture* output =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING, bitmap.width, bitmap.rows);

    if (bitmap.width == 0 || bitmap.rows == 0) {
        return nullptr;
    }
    void* buffer;
    int pitch;
    SDL_LockTexture(output, NULL, &buffer, &pitch);

    unsigned char* src_pixels = bitmap.buffer;
    unsigned int* target_pixels = reinterpret_cast<unsigned int*>(buffer);

    SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

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

void FreeTypeStrokeScene::DrawGlyph(FT_Glyph glyph, const SDL_Color& color, int& x,
    const int& baseline, SDL_Renderer* renderer) {
    FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, 0);
    
    FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph)glyph;

    if (glyph_bitmap->bitmap.width != 0 && glyph_bitmap->bitmap.rows != 0) {
        SDL_Texture* glyph_texture =
            CreateTextureFromFT_Bitmap(renderer, glyph_bitmap->bitmap, color);

        SDL_Rect dest;
        SDL_QueryTexture(glyph_texture, NULL, NULL, &dest.w, &dest.h);
        dest.x = x + (glyph_bitmap->left);
        dest.y = baseline - (glyph_bitmap->top);

        SDL_SetTextureBlendMode(glyph_texture, SDL_BLENDMODE_BLEND);

        SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);
        SDL_DestroyTexture(glyph_texture);
    }
    x += (glyph_bitmap->root.advance.x >> 16);

    
}

void FreeTypeStrokeScene::DrawText(const std::wstring& text, const SDL_Color& color,
    const int& baseline, const int& x_start, const FT_Face& face,
    const FT_Stroker& stroker, const SDL_Color& border_color,
    SDL_Renderer*& renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Pass#1 Border
    int x = x_start;
    for (unsigned int i = 0; i < text.length(); i++) {
        FT_Load_Char(face, text[i], FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);

        FT_Glyph glyph;
        FT_Get_Glyph(face->glyph, &glyph);
        FT_Glyph_StrokeBorder(&glyph, stroker, false, true);

        DrawGlyph(glyph, border_color, x, baseline, renderer);
        FT_Done_Glyph(glyph);
    }

    // Pass#2 Glyph
    x = x_start;
    for (unsigned int i = 0; i < text.length(); i++) {
        FT_Load_Char(face, text[i], FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);

        FT_Glyph glyph;
        FT_Get_Glyph(face->glyph, &glyph);

        DrawGlyph(glyph, color, x, baseline, renderer);
        FT_Done_Glyph(glyph);
    }
}