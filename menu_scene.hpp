#ifndef MENU_SCENE_HPP
#define MENU_SCENE_HPP

#include <SDL2/SDL.h>
#include "scene.hpp"

class MenuScene : public Scene {
public:
	virtual bool Init(SDL_Renderer* renderer);
	virtual void Tick(SDL_Renderer* renderer);
	virtual void Cleanup(SDL_Renderer* renderer);
};

#endif