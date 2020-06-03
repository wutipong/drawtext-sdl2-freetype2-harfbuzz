#include "menu_scene.hpp"

#include <imgui.h>

#include "freetype_harfbuzz_scene.hpp"
#include "freetype_scene.hpp"
#include "freetype_stroke_scene.hpp"

bool MenuScene::Init(SDL_Renderer *renderer) { return true; }

void MenuScene::Tick(SDL_Renderer *renderer) {
  ImGui::Begin("Menu");
  if (ImGui::Button("FreeType")) {
    ChangeScene<FreeTypeScene>(renderer);
  }
  if (ImGui::Button("FreeType + Outline")) {
    ChangeScene<FreeTypeStrokeScene>(renderer);
  }
  if (ImGui::Button("FreeType + Harfbuzz")) {
    ChangeScene<FreeTypeHarfbuzzScene>(renderer);
  }
  ImGui::End();
}

void MenuScene::Cleanup(SDL_Renderer *renderer) {}