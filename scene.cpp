#include "scene.hpp"

#include "menu_scene.hpp"

std::unique_ptr<Scene> Scene::currentScene = std::make_unique<MenuScene>();

void Scene::TickCurrent(const Context &context) {
  if (currentScene == nullptr)
    return;

  currentScene->Tick(context);
}