#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

const std::wstring text = L"ก็คงไม่เป็นไร ก็ขอให้โชคดี!!";
const int WIDTH = 1280;
const int HEIGHT = 720;

SDL_Texture* CreateTextureFromFT_Bitmap(SDL_Renderer* renderer,
	const FT_Bitmap& bitmap,
	const SDL_Color& color)
{
	SDL_Texture* output = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		bitmap.width,
		bitmap.rows);

	void *buffer;
	int pitch;
	SDL_LockTexture(output, NULL, &buffer, &pitch);

	unsigned char *src_pixels = bitmap.buffer;
	unsigned int *target_pixels = reinterpret_cast<unsigned int*>(buffer);

	SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

	for (int y = 0; y < bitmap.rows; y++)
	{
		for (int x = 0; x < bitmap.width; x++)
		{
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

void DrawGlyph(FT_Glyph glyph,
	const SDL_Color& color,
	int&x,
	const int& baseline,
	SDL_Renderer* renderer)
{
	FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, 0);

	FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph)glyph;

	SDL_Texture* glyph_texture
		= CreateTextureFromFT_Bitmap(renderer, glyph_bitmap->bitmap, color);

	SDL_Rect dest;
	SDL_QueryTexture(glyph_texture, NULL, NULL, &dest.w, &dest.h);
	dest.x = x + (glyph_bitmap->left);
	dest.y = baseline - (glyph_bitmap->top);

	SDL_SetTextureBlendMode(glyph_texture, SDL_BLENDMODE_BLEND);

	SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);

	x += (glyph_bitmap->root.advance.x >> 16);

	SDL_DestroyTexture(glyph_texture);
}

void DrawText(const std::wstring& text,
	const SDL_Color& color,
	const int& baseline,
	const int& x_start,
	const FT_Face& face,
	const FT_Stroker& stroker,
	const SDL_Color& border_color,
	SDL_Renderer*& renderer)
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	//Pass#1 Border
	int x = x_start;
	for (unsigned int i = 0; i < text.length(); i++)
	{
		FT_Load_Char(face, text[i], FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);

		FT_Glyph glyph;
		FT_Get_Glyph(face->glyph, &glyph);
		FT_Glyph_StrokeBorder(&glyph, stroker, false, true);

		DrawGlyph(glyph, border_color, x, baseline, renderer);
		FT_Done_Glyph(glyph);
	}

	//Pass#2 Glyph
	x = x_start;
	for (unsigned int i = 0; i < text.length(); i++)
	{
		FT_Load_Char(face, text[i], FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING);

		FT_Glyph glyph;
		FT_Get_Glyph(face->glyph, &glyph);

		DrawGlyph(glyph, color, x, baseline, renderer);
		FT_Done_Glyph(glyph);
	}
}

int main(int argc, char **argv) {
	if (argc != 2)
		return -1;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Test Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH,
		HEIGHT,
		0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	FT_Library library;
	FT_Init_FreeType(&library);

	FT_Face face;
	FT_New_Face(library, argv[1], 0, &face);
	FT_Set_Pixel_Sizes(face, 0, 64);

	FT_Stroker stroker;
	FT_Stroker_New(library, &stroker);
	FT_Stroker_Set(stroker,
		2 << 6,
		FT_STROKER_LINECAP_ROUND,
		FT_STROKER_LINEJOIN_ROUND, 0);

	SDL_Color border_color;
	border_color.r = 0xEE;
	border_color.g = 0x10;
	border_color.b = 0xCC;

	SDL_Color color;
	color.r = 0x30;
	color.g = 0x25;
	color.b = 0xCC;

	while (true)
	{
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)	break;
		}

		SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
		SDL_RenderClear(renderer);

		DrawText(text,
			color,
			300,
			20,
			face,
			stroker,
			border_color,
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
