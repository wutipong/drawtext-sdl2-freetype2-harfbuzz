#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include <string>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

std::wstring text = L"ค่ำคืน มึดมิด กี่โมง ไม่รู้ 555 มึนซะแล้วสินะ";

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

void DrawText(const std::wstring& text,
		const unsigned int& color,
		const int& baseline,
		const int& x_start,
		const FT_Face& face,
		hb_font_t* hb_font,
		SDL_Renderer*& renderer) {

	hb_buffer_t *buffer = hb_buffer_create();

	hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(buffer, HB_SCRIPT_THAI);

	hb_buffer_add_utf16(buffer, (unsigned short*) (text.c_str()), text.length(),
			0, text.length());
	hb_shape(hb_font, buffer, NULL, 0);

	unsigned int glyph_count = hb_buffer_get_length(buffer);
	hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
	hb_glyph_position_t *glyph_positions = hb_buffer_get_glyph_positions(buffer,
			NULL);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	int x = x_start;

	for (unsigned int i = 0; i < glyph_count; i++) {
		FT_Load_Glyph(face, glyph_infos[i].codepoint, FT_LOAD_RENDER);

		SDL_Surface* surface = NULL;
		CreateSurfaceFromFT_Bitmap(face->glyph->bitmap, color, surface);
		SDL_Texture* glyph_texture = SDL_CreateTextureFromSurface(renderer,
				surface);

		SDL_Rect dest;
		dest.x = x + (face->glyph->metrics.horiBearingX >> 6) + (glyph_positions[i].x_offset >> 6);

		dest.y = baseline - ((face->glyph->metrics.horiBearingY >> 6) + (glyph_positions[i].y_offset >> 6));
		dest.w = surface->w;
		dest.h = surface->h;

		SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);

		x += (glyph_positions[i].x_advance >> 6);

		SDL_DestroyTexture(glyph_texture);
		SDL_FreeSurface(surface);
	}

	hb_buffer_destroy(buffer);
}

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Test Window",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	FT_Library library;
	FT_Init_FreeType(&library);

	FT_Face face;
	FT_New_Face(library, "./font/ThaiSansNeue-Regular.otf", 0, &face);
	FT_Set_Pixel_Sizes(face, 0, 64);

	hb_font_t* hb_font = hb_ft_font_create(face, 0);

	while (true) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				break;
		}

		SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
		SDL_RenderClear(renderer);

		DrawText(text, 0xffffffff, 300, 30, face, hb_font, renderer);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	hb_font_destroy(hb_font);
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
