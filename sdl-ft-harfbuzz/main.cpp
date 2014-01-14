#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

std::wstring text = L"ปักคมมีดกรีดกลางใจ แช่เกลือใว้ให้ตายทั้งเป็น";
const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

struct Font {
	FT_Face face;
	hb_font_t* hb_font;
};

void CreateFont(const FT_Library& library, 
				const std::string& path,
				const int& size, Font& font) 
{
	FT_New_Face(library, path.c_str(), 0, &font.face);
	FT_Set_Pixel_Sizes(font.face, 0, size);

	font.hb_font = hb_ft_font_create(font.face, 0);
}

void DestroyFont(Font& font) 
{
	hb_font_destroy(font.hb_font);
	FT_Done_Face(font.face);
}

void CreateSurfaceFromFT_Bitmap(const FT_Bitmap& bitmap,
								const unsigned int& color, 
								SDL_Surface*& output) 
{
	output = SDL_CreateRGBSurface(0, 
		bitmap.width, 
		bitmap.rows, 
		32, 
		0x000000ff,
		0x0000ff00, 
		0x00ff0000, 
		0xff000000);

	SDL_FillRect(output, NULL, color);

	SDL_LockSurface(output);

	unsigned char *src_pixels = bitmap.buffer;
	unsigned int *target_pixels = reinterpret_cast<unsigned int*>(output->pixels);

	for (int i = 0; i < bitmap.rows; i++) 
	{
		for (int j = 0; j < bitmap.width; j++) 
		{
			unsigned int pixel = target_pixels[i * output->w + j];
			unsigned int alpha = src_pixels[i * bitmap.pitch + j];

			pixel &= ((alpha << 24) | 0x00ffffff);

			target_pixels[i * output->w + j] = pixel;
		}
	}
	SDL_UnlockSurface(output);
}

void CalculateSurfaceBound(	hb_glyph_info_t *glyph_infos,
							hb_glyph_position_t *glyph_positions, 
							const unsigned int& glyph_count,
							const FT_Face& face, 
							SDL_Rect& rect, 
							const FT_Int32& flags = FT_LOAD_DEFAULT) 
{
	int width = 0;
	int above_base_line = 0;
	int below_base_line = 0;

	for (unsigned int i = 0; i < glyph_count; i++) 
	{
		FT_Load_Glyph(face, glyph_infos[i].codepoint, FT_LOAD_DEFAULT | flags);
		width += (glyph_positions[i].x_advance >> 6);
		int bearing = 
			(face->glyph->metrics.horiBearingY >> 6) + (glyph_positions[i].y_offset >> 6);

		if (bearing > above_base_line)
			above_base_line = bearing;

		int height_minus_bearing = (face->glyph->metrics.height >> 6) - bearing;
		if (height_minus_bearing > below_base_line)
			below_base_line = height_minus_bearing;
	}

	rect.x = 0;
	rect.y = -above_base_line;
	rect.w = width;
	rect.h = above_base_line + below_base_line;

}

void CreateTexture(const std::wstring& text, 
					const unsigned int& color,
					const Font& font, 
					SDL_Renderer*& renderer, 
					SDL_Texture*& target,
					SDL_Rect& rect, 
					const FT_Int32& flags = FT_LOAD_DEFAULT) 
{
	hb_buffer_t *buffer = hb_buffer_create();

	hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(buffer, HB_SCRIPT_THAI);

	hb_buffer_add_utf16(buffer, 
		(unsigned short*)(text.c_str()), 
		text.length(),
		0, text.length());

	hb_shape(font.hb_font, buffer, NULL, 0);

	unsigned int glyph_count = hb_buffer_get_length(buffer);
	hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
	hb_glyph_position_t *glyph_positions = hb_buffer_get_glyph_positions(buffer, NULL);

	CalculateSurfaceBound(glyph_infos, 
		glyph_positions, 
		glyph_count, 
		font.face,
		rect, 
		flags);

	target = SDL_CreateTexture(renderer, 
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_TARGET, 
		rect.w, 
		rect.h);

	SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, target);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_RenderFillRect(renderer, NULL);

	int baseline = -rect.y;
	int x = 0;

	for (unsigned int i = 0; i < glyph_count; i++) 
	{
		FT_Load_Glyph( font.face, 
			glyph_infos[i].codepoint,
			FT_LOAD_RENDER | flags);

		SDL_Surface* surface = NULL;
		CreateSurfaceFromFT_Bitmap(font.face->glyph->bitmap, color, surface);
		SDL_Texture* glyph_texture = SDL_CreateTextureFromSurface(renderer,	surface);

		SDL_Rect dest;
		dest.x = x + (font.face->glyph->metrics.horiBearingX >> 6) + (glyph_positions[i].x_offset >> 6);

		dest.y = baseline - (font.face->glyph->metrics.horiBearingY >> 6) - (glyph_positions[i].y_offset >> 6);

		dest.w = surface->w;
		dest.h = surface->h;

		SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);

		x += (glyph_positions[i].x_advance >> 6);

		SDL_DestroyTexture(glyph_texture);
		SDL_FreeSurface(surface);
	}

	hb_buffer_destroy(buffer);

	SDL_SetRenderTarget(renderer, NULL);
}

int main(int argc, char **argv) 
{
	if (argc != 2) return -1;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("SDL2 - FreeType 2 - HarfBuzz",
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		WIDTH, 
		HEIGHT, 
		0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	FT_Library library;
	FT_Init_FreeType(&library);

	Font font;
	CreateFont(library, argv[1], 64, font);

	SDL_Rect rect, dest;

	while (true) 
	{
		SDL_Event event;
		if (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT)	break;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_Texture* texture = NULL;
		CreateTexture(text, 
			0xffffffff, 
			font, 
			renderer, 
			texture, 
			rect,
			FT_LOAD_NO_HINTING);

		dest = rect;
		dest.y += 400;
		SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
		SDL_RenderFillRect(renderer, NULL);

		SDL_RenderCopy(renderer, texture, NULL, &dest);

		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(texture);
		SDL_Delay(10);
	}

	DestroyFont(font);

	FT_Done_FreeType(library);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
