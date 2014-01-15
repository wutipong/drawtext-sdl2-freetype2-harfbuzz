#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

const std::wstring TEXT = L"เก็บใจ เก็บไว้มานาน เก็บมันคล้าย ๆ รอใคร";

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

void DrawText(const std::wstring& text, 
			  const SDL_Color& color,
			  const int& baseline, 
			  const int& x_start, 
			  const FT_Face& face,
			  SDL_Renderer* renderer) 
{
	int x = x_start;

	for (unsigned int i = 0; i < text.length(); i++) 
	{
		FT_Load_Char(face, text[i], FT_LOAD_RENDER);

		SDL_Texture* glyph_texture = CreateTextureFromFT_Bitmap(renderer, face->glyph->bitmap, color);
		
		SDL_Rect dest;
		SDL_QueryTexture(glyph_texture, NULL, NULL, &dest.w, &dest.h);
		dest.x = x + (face->glyph->metrics.horiBearingX >> 6);
		dest.y = baseline - (face->glyph->metrics.horiBearingY >> 6);
		
		SDL_SetTextureBlendMode(glyph_texture, SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);

		x += (face->glyph->metrics.horiAdvance >> 6);
		SDL_DestroyTexture(glyph_texture);
	}
}

int main(int argc, char **argv) 
{
	if(argc!=2) return -1;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow (	
		"Test Window",
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		WIDTH, 
		HEIGHT, 
		0);
	SDL_Renderer* renderer;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	FT_Library library;
	FT_Init_FreeType(&library);

	FT_Face face;
	FT_New_Face(library, argv[1], 0, &face);
	FT_Set_Pixel_Sizes(face, 0, 64);

	SDL_Color color;
	color.r = 0x80;
	color.g = 0xff;
	color.b = 0xaa;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	while (true) 
	{
		SDL_Event event;
		if (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT)	break;
		}

		SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
		SDL_RenderClear(renderer);

		DrawText(TEXT, color, 300, 300, face, renderer);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
