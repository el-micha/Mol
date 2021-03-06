#ifndef SIM
#define SIM

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
#include <cmath>

class Sim
{
private:
	//Simulation Resolution:
	int GRID_WIDTH;
	int GRID_HEIGHT;
	long GRID_LENGTH;
	//Draw Resolution:
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	int CELL_WIDTH;
	int CELL_HEIGHT;

	SDL_Window * window = NULL;
	SDL_Renderer * renderer = NULL;
	
public:
	Sim(int, int);
	~Sim();
	void run();
	bool running;
	bool initSDL();
	void close();
	SDL_Texture * loadTexture(std::string path);
};

#endif
