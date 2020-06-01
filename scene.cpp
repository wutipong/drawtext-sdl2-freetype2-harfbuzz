#include "scene.hpp"

#include "menu_scene.hpp"

std::unique_ptr<Scene> Scene::currentScene = std::make_unique<MenuScene>();

void Scene::TickCurrent(SDL_Renderer* renderer) {
	if (currentScene == nullptr)
		return;
	currentScene->Tick(renderer);
}