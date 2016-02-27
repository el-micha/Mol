#include "Sim.h"
#include "Concentration.h"
#include <iostream>
#include <cstdlib>
#include <time.h>

Sim::Sim(int gridWidth, int gridHeight)
{
	GRID_WIDTH = gridWidth;
	GRID_HEIGHT = gridHeight;
	GRID_LENGTH = gridWidth * gridHeight;

	SCREEN_WIDTH = 1024 + 512;
	SCREEN_HEIGHT = 512 + 256;

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
	Concentration mol2 = Concentration(GRID_WIDTH, GRID_HEIGHT);

	long max = 1000000;
	mol.setCell(40000, GRID_WIDTH*(GRID_HEIGHT+1) / 2);
	//mol.randomize(100, 1000, 10000);

	if (!initSDL())
	{
		std::cout << "Could not init Sim." << std::endl;
	}

	running = true;

	SDL_Event event;
	int counter = 0;
	while (running)
	{
		clock_t s0 = clock();
		counter++;
		//Handle events on queue
		while (SDL_PollEvent(&event) != 0)
		{
			//User requests quit
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				int x = event.button.x;
				int y = event.button.y;

				int cellwidth = SCREEN_WIDTH / GRID_WIDTH;
				int cellheight = SCREEN_HEIGHT / GRID_HEIGHT;

				x = x / CELL_WIDTH;
				y = y / CELL_HEIGHT;

				//std::cout << mol.getCell(x,y) << std::endl;
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
			unsigned long b = mol2.getCell(i);
			

			SDL_Rect rect = { y*CELL_WIDTH, x*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT };
			SDL_SetRenderDrawColor(renderer, (1 * a) % 256, (1 * a) % 256, (1 * a) % 255, 255);
			SDL_RenderFillRect(renderer, &rect);
		}

		//Update screen
		SDL_RenderPresent(renderer);

		clock_t e1 = clock();
		//std::cout << "Render Time: " << e1 - s1 << std::endl;

		//mol.print();
		//SDL_Delay(100);
		clock_t start = clock();
		//mol.diffuse();
		mol.tick(counter);
		mol2.tick(counter);
		clock_t end = clock();
		//std::cout << "Diffuse Time: " << end - start << std::endl;
		
		mol.randomize(1, 0, 1000);
		//mol2.randomize(1, 0, 255);
		//mol.setCell(25, (GRID_HEIGHT+0.8)*GRID_WIDTH/2);
		//mol.setCell(255, (GRID_HEIGHT + 1.2)*GRID_WIDTH / 2);

		clock_t e0 = clock();
		std::cout << "Tick Time: " << e0 - s0 << std::endl;
		if (counter%30 == 0)
			std::cout << mol.total() << std::endl;
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