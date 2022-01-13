#define SDL_MAIN_HANDLED
#include "graphics.h"
#include "Map.h"
#include <chrono>

const float FOV = M_PI / 3.0f;


float syncTime = 0.0f;
const int MAP_WIDTH = 16;
const int MAP_HEIGHT = 16;
bool running = true;

float lookAngle = 3.14159f;
vec2f playerPos;



void HandleEvents()
{
	SDL_Event e;
	SDL_PollEvent(&e);
	if (e.type == SDL_QUIT) {
		running = false;
	}
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	if (keyboard[SDL_SCANCODE_A]) {
		lookAngle -= (1.5f*syncTime);
	}
	if (keyboard[SDL_SCANCODE_D]) {
		lookAngle += (1.5f * syncTime);
	}
	if (keyboard[SDL_SCANCODE_UP]) {
		float oldx = playerPos.x;
		float oldy = playerPos.y;
		playerPos.x += cosf(lookAngle)*1.5f * syncTime;
		playerPos.y += sinf(lookAngle) * 1.5f * syncTime;
		if (Map[(int)playerPos.x][(int)playerPos.y] == 1) {
			playerPos.x = oldx;
			playerPos.y = oldy;
		}
	}
	if (keyboard[SDL_SCANCODE_DOWN]) {
		float oldx = playerPos.x;
		float oldy = playerPos.y;
		
		playerPos.x -= cosf(lookAngle) * 1.5f * syncTime;
		playerPos.y -= sinf(lookAngle) * 1.5f * syncTime;
		if (Map[(int)playerPos.x][(int)playerPos.y] == 1) {
			playerPos.x = oldx;
			playerPos.y = oldy;
		}
	}
}

void raycastDDA(SDL_Renderer *rend)
{
	
	//column loop
	for (int x = 0; x < screenWidth; x++)
	{
		vec2f rayBegin = { playerPos.x,playerPos.y };
		vec2i rayOnMap = { rayBegin.x,rayBegin.y };
		float rayAngle = (lookAngle - FOV / 2) + (((float)x / (float)screenWidth) * FOV);
		vec2f rayDir;
		rayDir = {cosf(rayAngle), sinf(rayAngle)};
		vec2f rayUnitStepSize = { sqrtf(1 + (rayDir.y / rayDir.x)*(rayDir.y/rayDir.x)),sqrtf(1 + (rayDir.x / rayDir.y)*(rayDir.x/rayDir.y)) };
		vec2f rayLength1D = { 0.0f,0.0f };
		vec2i step = { 0,0 };
		int side = 0; //0 means x side 1 means y side
		if (rayDir.x < 0) {
			step.x = -1;
			rayLength1D.x = (rayBegin.x - (float)rayOnMap.x) * rayUnitStepSize.x;
		}
		else {
			step.x = 1;
			rayLength1D.x = ((float)(rayOnMap.x+1) - rayBegin.x) * rayUnitStepSize.x;
		}

		if (rayDir.y < 0) {
			step.y = -1;
			rayLength1D.y = (rayBegin.y - (float)rayOnMap.y) * rayUnitStepSize.y;
		}
		else {
			step.y = 1;
			rayLength1D.y = ((float)(rayOnMap.y + 1) - rayBegin.y) * rayUnitStepSize.y;
		}
		bool wallHit = false;
		float distFromWall = 0.0f;
		while (!wallHit && (distFromWall < MAP_HEIGHT))
		{
			if (rayLength1D.x < rayLength1D.y)//Move in x
			{
				rayOnMap.x += step.x;
				distFromWall = rayLength1D.x;
				rayLength1D.x += rayUnitStepSize.x;
				side = 0;
			}
			else
			{
				rayOnMap.y += step.y;
				distFromWall = rayLength1D.y;
				rayLength1D.y += rayUnitStepSize.y;
				side = 1;
			}
			
			//check whether the ray has gone out of bounds
			if (rayOnMap.x < 0 || rayOnMap.x >= MAP_WIDTH || rayOnMap.y < 0 || rayOnMap.y > MAP_HEIGHT)
			{
				distFromWall = MAP_HEIGHT;
				wallHit = true;
			}
			else  //else check if the ray has hit a wall
			{
				if (Map[rayOnMap.x][rayOnMap.y] == 1) {
					wallHit = true;
				}
			}

		}
		//Fisheye correction
		float beta = lookAngle - rayAngle;
		if (beta < 0) {
			beta += (2 * M_PI);
		}
		else if (beta > 2 * M_PI) {
			beta -= (2 * M_PI);
		}
		distFromWall *= cosf(beta);
		
		
		int lineHeight = (int)(screenWidth / distFromWall);
		int ceilingPart = (screenHeight - lineHeight) / 2;
		if (ceilingPart < 0) ceilingPart = 0;
		int floorPart = (screenHeight + lineHeight) / 2;
		if (floorPart >= screenHeight) floorPart = screenHeight - 1;

		if (side == 0) {
			SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
		}
		else {
			SDL_SetRenderDrawColor(rend, 0, 0, 127, 255);
		}
		SDL_RenderDrawLine(rend, x, ceilingPart, x, floorPart);
	}

}
int main()
{
	init("DDA Raycaster",100,100);
	playerPos.x = 8.0f;
	playerPos.y = 8.0f;
	auto tf = std::chrono::system_clock::now();
	auto ti = std::chrono::system_clock::now();
	while (running)
	{
		tf = std::chrono::system_clock::now();
		std::chrono::duration <float> elT = tf - ti;
		ti = tf;
		syncTime = elT.count();
		SDL_SetRenderDrawColor(renderer, 100 , 100, 100,255);
		SDL_RenderClear(renderer);
		//Process here
		HandleEvents();
		raycastDDA(renderer);
	
		SDL_RenderPresent(renderer);
	}
	clean();
	return 0;
}