#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

#include "scene.hpp"
#include <SDL2/SDL.h>

class MenuScene : public Scene {
public:
  bool Init(const Context &context)override;
  void Tick(const Context &context)override;
  void Cleanup(const Context &context)override;
};

#endif