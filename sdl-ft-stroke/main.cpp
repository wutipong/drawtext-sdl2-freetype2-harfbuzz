#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include <string>

std::wstring text = L"ก็คงไม่เป็นไร ก็ขอให้โชคดี!!";

void CreateSurfaceFromFT_Bitmap(const FT_Bitmap& bitmap,
		const unsigned int& color, SDL_Surface*& output) {
	output = SDL_CreateRGBSurface(0, bitmap.width, bitmap.rows, 32, 0x000000ff,
			0x0000ff00, 0x00ff0000, 0xff000000);

	SDL_FillRect(output, NULL, color);

	SDL_LockSurface(output);

	unsigned char *src_pixels = bitmap.buffer;
	unsigned int *target_pixels =
			reinterpret_cast<unsigned int*>(output->pixels);

	for (int i = 0; i < bitmap.rows; i++) {
		for (int j = 0; j < bitmap.width; j++) {
			unsigned int pixel = target_pixels[i * output->w + j];
			unsigned int alpha = src_pixels[i * bitmap.pitch + j];

			pixel &= (alpha << 24) | 0x00ffffff;

			target_pixels[i * output->w + j] = pixel;
		}
	}
	SDL_UnlockSurface(output);
}

void DrawGlyph(FT_Glyph glyph, const unsigned int& color, int&x, const int& baseline, SDL_Renderer* renderer){
	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, 0);

	FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph) glyph;
	SDL_Surface* surface = NULL;
	CreateSurfaceFromFT_Bitmap(glyph_bitmap->bitmap, color, surface);

	SDL_Texture* glyph_texture
		= SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect dest;
	dest.x = x + (glyph_bitmap->left);

	dest.y = baseline - (glyph_bitmap->top);
	dest.w = surface->w;
	dest.h = surface->h;

	SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);

	x += (glyph_bitmap->root.advance.x >> 16);

	SDL_DestroyTexture(glyph_texture);
	SDL_FreeSurface(surface);
}

void DrawText(const std::wstring& text, const unsigned int& color,
		const int& baseline, const int& x_start, const FT_Face& face,
		const FT_Stroker& stroker, const unsigned int& border_color,
		SDL_Renderer*& renderer) {

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	//Pass#1 Border
	int x = x_start;
	for (unsigned int i = 0; i < text.length(); i++) {
		FT_Load_Char(face, text[i], FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);

		FT_Glyph glyph;
		FT_Get_Glyph(face->glyph, &glyph);
		FT_Glyph_StrokeBorder(&glyph, stroker, false, true);

		DrawGlyph(glyph, border_color, x, baseline, renderer);
		FT_Done_Glyph(glyph);
	}

	//Pass#2 Glyph
	x = x_start;
	for (unsigned int i = 0; i < text.length(); i++) {
		FT_Load_Char(face, text[i], FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);

		FT_Glyph glyph;
		FT_Get_Glyph(face->glyph, &glyph);

		DrawGlyph(glyph, color, x, baseline, renderer);
		FT_Done_Glyph(glyph);
	}
}

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Test Window",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	FT_Library library;
	FT_Init_FreeType(&library);

	FT_Face face;
	FT_New_Face(library, "./font/ThaiSansNeue-ExtraLight.otf", 0, &face);
	FT_Set_Pixel_Sizes(face, 0, 64);

	FT_Stroker stroker;
	FT_Stroker_New(library, &stroker);
	FT_Stroker_Set(stroker,
			2 << 6,
			FT_STROKER_LINECAP_ROUND,
			FT_STROKER_LINEJOIN_ROUND, 0);

	while (true) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				break;
		}

		SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
		SDL_RenderClear(renderer);

		DrawText(text, 0xff0000ff, 300, 20, face, stroker, 0xffffffff,
				renderer);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
