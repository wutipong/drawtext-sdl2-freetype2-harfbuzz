#include <SDL2/SDL.h>
#include <imgui.h>

#include "imgui_impl_sdl.h"
#include "imgui_sdl/imgui_sdl.h"
#include "menu_scene.hpp"
#include "scene.hpp"

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

  ImGuiSDL::Initialize(renderer, WIDTH, HEIGHT);
  ImGui_ImplSDL2_Init(window);

  while (true) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        break;
    }

    SDL_SetRenderDrawColor(renderer, 0x50, 0x82, 0xaa, 0xff);
    SDL_RenderClear(renderer);

    ImGui::NewFrame();

    Scene::TickCurrent(renderer);

    ImGui_ImplSDL2_UpdateMousePosAndButtons();
    ImGui_ImplSDL2_UpdateMouseCursor();
    ImGui_ImplSDL2_UpdateGamepads();

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }

  ImGui_ImplSDL2_Shutdown();
  ImGuiSDL::Deinitialize();
  ImGui::DestroyContext();

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}