#pragma once
#include "SDL.h"
#include <iostream>
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int screenWidth = 800;
int screenHeight = 600;

void init(const char* title, int X, int Y)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Problem initializing subsystems!\n";
		return;
	}
	else {
		std::cout << "Subsystems initialized!\n";
	}
	window = SDL_CreateWindow(title, X, Y, screenWidth, screenHeight, 0);
	if (!window) {
		std::cout << "Error!Failed to create window!\n";
		return;
	}
	else {
		std::cout << "Window created!\n";
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		std::cout << "Error!Failed to create a renderer!\n";
		return;
	}
	else {
		std::cout << "Renderer created!\n";
	}
}

void clean() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

//Vector of 2 floats to denote player position

struct vec2f
{
	float x;
	float y;
};


//Vector of two integers to denote player position on the integer grid
struct vec2i
{
	int x;
	int y;
};