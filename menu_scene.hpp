#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

#include "scene.hpp"
#include <SDL2/SDL.h>

class MenuScene : public Scene {
public:
  virtual bool Init(SDL_Renderer *renderer);
  virtual void Tick(SDL_Renderer *renderer);
  virtual void Cleanup(SDL_Renderer *renderer);
};

#endif