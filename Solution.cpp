#include "Solution.h"
#include "Concentration.h"

Solution::Solution(int maxNum, int width, int height)
{
	maxConcentrations = maxNum;
	substances = new Concentration*[maxNum];
	
	for (int i = 0; i < maxNum; i++)
	{
		substances[i] = new Concentration(width, height);
	}
	activeConcentrations = maxNum;
}

Solution::~Solution()
{
	for (int i = 0; i < activeConcentrations; i++)
	{
		delete substances[i];
	}
	delete substances;
}

void Solution::tick(int counter)
{
	for (int i = 0; i < activeConcentrations; i++)
	{
		substances[i]->tick(counter);
	}
}

void Solution::randomize(long intensity)
{
	for (int i = 0; i < activeConcentrations; i++)
	{
		substances[i]->randomize(1, 0, intensity);
	}
}

long Solution::getCell(int gridNum, long pos)
{
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