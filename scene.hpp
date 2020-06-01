#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL2/SDL.h>
#include <memory>

class Scene {
public:
  virtual bool Init(SDL_Renderer *renderer) = 0;
  virtual void Tick(SDL_Renderer *renderer) = 0;
  virtual void Cleanup(SDL_Renderer *renderer) = 0;

  static void TickCurrent(SDL_Renderer *renderer);
  template <class T> static void ChangeScene(SDL_Renderer* renderer);

private:
  static std::unique_ptr<Scene> currentScene;
};

template <class T> 
void Scene::ChangeScene(SDL_Renderer* renderer) {
	auto newScene = std::make_unique<T>();
	
	if (!newScene->Init(renderer))
		return;

	currentScene->Cleanup(renderer);
	currentScene = std::move(newScene);
}

#endif
