#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

wchar_t* text = L"รี่ร่อร่าเริง";

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Test Window",
   		 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    FT_Library library;
    FT_Init_FreeType(&library);
    FT_Face face;
    FT_New_Face(library, "./font/ThaiSansNeue-SemiBold.otf", 0, &face);
    FT_Set_Pixel_Sizes(face, 0, 64);

    hb_font_t* hb_font = hb_ft_font_create(face, 0);
    hb_buffer_t *buffer = hb_buffer_create();

    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer, HB_SCRIPT_THAI);

    hb_buffer_add_utf16(buffer, reinterpret_cast<unsigned short*>(text),
   		 wcslen(text), 0, wcslen(text));
    hb_shape(hb_font, buffer, NULL, 0);

    unsigned int glyph_count = hb_buffer_get_length(buffer);

    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);

    hb_glyph_position_t *glyph_positions = hb_buffer_get_glyph_positions(buffer,
   		 NULL);

    for(unsigned int i = 0; i< glyph_count; i++) {
   	 std::cout<<glyph_infos[i].codepoint
   			 <<"\t"
   			 <<(glyph_positions[i].x_offset >>6)
   			 <<","
   			 <<(glyph_positions[i].y_offset >>6);

   	 std::cout<<std::endl;
    }

    std::cout<<std::endl;

    for(unsigned int i = 0; i<glyph_count; i++) {
   	 FT_UInt glyph_index = FT_Get_Char_Index( face, text[i] );
   	 std::cout<<glyph_index<<std::endl;
    }
    while (true) {
   	 SDL_Event event;
   	 if (SDL_PollEvent(&event)) {
   		 if (event.type == SDL_QUIT)
   			 break;
   	 }

   	 SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
   	 SDL_RenderClear(renderer);

   	 SDL_RenderPresent(renderer);
   	 SDL_Delay(1);
    }
    hb_buffer_destroy(buffer);
    hb_font_destroy(hb_font);

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
