#include "Sim.h"
#include "Concentration.h"
#include <iostream>
#include <cstdlib>
#include <time.h>

Sim::Sim(int gridWidth, int gridHeight)
{
	GRID_WIDTH = gridWidth;
	GRID_HEIGHT = gridHeight;

	SCREEN_WIDTH = 1024;
	SCREEN_HEIGHT = 512;

	running = false;


}

Sim::~Sim()
{
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void Sim::run()
{
	Concentration mol = Concentration(GRID_WIDTH, GRID_HEIGHT);
	//mol.setCell(1000, GRID_WIDTH*(GRID_HEIGHT+1) / 2);
	mol.randomize(100, 1000, 10000);

	if (!initSDL())
	{
		std::cout << "Could not init Sim." << std::endl;
	}

	running = true;

	SDL_Event event;
	while (running)
	{
		//Handle events on queue
		while (SDL_PollEvent(&event) != 0)
		{
			//User requests quit
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}
		//Simulate and draw here
		
		//Clear screen
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		CELL_WIDTH = (int)((float)SCREEN_WIDTH / GRID_WIDTH);
		CELL_HEIGHT = (int)((float)SCREEN_HEIGHT / GRID_HEIGHT);

		clock_t s1 = clock();

		for (long i = 0; i < GRID_WIDTH*GRID_HEIGHT; i++)
		{
			//x zeilennummer
			int x = ((double)i) / GRID_WIDTH;
			int y = i % GRID_WIDTH;
			
			unsigned long a = mol.getCell(i);
			
			SDL_Rect rect = { y*CELL_WIDTH, x*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT };
			SDL_SetRenderDrawColor(renderer, a % 256, 0 % 255, 0 % 255, 255);
			SDL_RenderFillRect(renderer, &rect);
		}

		//Update screen
		SDL_RenderPresent(renderer);

		clock_t e1 = clock();
		//std::cout << "Render Time: " << e1 - s1 << std::endl;

		//SDL_Delay(0);
		clock_t start = clock();
		//mol.diffuse();
		mol.tick(0);
		clock_t end = clock();
		std::cout << "Diffuse Time: " << end - start << std::endl;
		mol.randomize(1, 1000, 10000);
	}


}

bool Sim::initSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Error: SDL not initialized" << std::endl;
		return false;
	}
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		std::cout << "Error: Linear Filtering not enabled" << std::endl;
	}
	window = SDL_CreateWindow("SDLTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		std::cout << "Error: Window not initialized." << std::endl;
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		std::cout << "Error: Renderer not initialized" << std::endl;
		return false;
	}
	
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		std::cout<< "Error: SDL_image not initialized." << std::endl;
		return false;
	}

	return true;
}

SDL_Texture * Sim::loadTexture(std::string path)
{
	SDL_Texture * newTex = NULL;
	SDL_Surface * loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		std::cout << "Error: Surface not loaded." << std::endl;
		return NULL;
	}
	newTex = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	if (newTex == NULL)
	{
		std::cout << "Error: Texture not loaded." << std::endl;
		return NULL;
	}
	SDL_FreeSurface(loadedSurface);
	return newTex;
	//Don't forget to call SDL_DestroyTexture(*tex) 
}

void Sim::close()
{
	
}