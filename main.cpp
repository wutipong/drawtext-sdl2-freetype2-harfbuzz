#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include "menu_scene.hpp"
#include "scene.hpp"
#include "context.hpp"

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *window =
      SDL_CreateWindow("Test Window", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
  SDL_Renderer *renderer;
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("Sarabun-Regular.ttf", 20.0f, NULL,
                               io.Fonts->GetGlyphRangesThai());

  ImGui_ImplSDLRenderer_Init(renderer);
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);

  Context c{renderer, nullptr};
  FT_Init_FreeType(&c.ftLibrary);

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        break;
    }

    SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
    SDL_RenderClear(renderer);

    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    Scene::TickCurrent(c);

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }

  ImGui_ImplSDL2_Shutdown();
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui::DestroyContext();

  FT_Done_FreeType(c.ftLibrary);

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}