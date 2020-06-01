#include "menu_scene.hpp"

#include <imgui.h>

#include "freetype_scene.hpp"

bool MenuScene::Init(SDL_Renderer* renderer) {
	return true;
}

void MenuScene::Tick(SDL_Renderer* renderer) {
	ImGui::Begin("Menu"); 
	if (ImGui::Button("FreeType")) {
		ChangeScene<FreeTypeScene>(renderer);
	}
	ImGui::Button("FreeType + Outline");
	ImGui::End();
}

void MenuScene::Cleanup(SDL_Renderer* renderer) {

}