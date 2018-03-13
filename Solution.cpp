#include "Solution.h"

Solution::Solution(int maxNum, int width, int height)
{
	maxConcentrations = maxNum;
	activeConcentrations = maxNum;
	substances = new Concentration*[maxNum];
	
	for (int i = 0; i < maxNum; i++)
	{
		substances[i] = new Concentration(width, height);
	}
}

Solution::~Solution()
{
	for (int i = 0; i < activeConcentrations; i++)
	{
		delete substances[i];
	}
	delete substances;
}

long Solution::total(int substance)
{
	return substances[substance] -> total();
}

void Solution::tick(int counter)
{
	//todo: parallelize this
	for (int i = 0; i < activeConcentrations; i++)
	{
		substances[i]->tick(counter);
	}
}

void Solution::draw(SDL_Renderer* renderer)
{

}

void Solution::randomize(long intensity)
{
	for (int i = 0; i < activeConcentrations; i++)
	{
		substances[i]->randomize(1, intensity-1, intensity);
	}
}

long Solution::getCell(int gridNum, long pos)
{
	if (gridNum > activeConcentrations)
	{
		std::cout << "solution:getCell gridNum > active Concentrations. Looping back" << std::endl;
		gridNum = gridNum % activeConcentrations;
	}
	return substances[gridNum]->getCell(pos);
}

int Solution::getActiveConcentrations()
{
	return activeConcentrations;
}
int Solution::getMaxConcentrations()
{
	return maxConcentrations;
}