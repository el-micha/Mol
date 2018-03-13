#ifndef SOLUTION
#define SOLUTION

#include <iostream>
#include <SDL.h>
#include "Concentration.h"

class Solution
{
private:
	Concentration ** substances;
	int maxConcentrations;
	int activeConcentrations;
public:
	Solution(int maxNum, int width, int height);
	~Solution();
	void tick(int);
	void draw(SDL_Renderer *);
	void randomize(long intensity);
	long getCell(int gridNum, long pos);
	int getActiveConcentrations();
	int getMaxConcentrations();
	long total(int);
};

#endif