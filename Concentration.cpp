#include "Concentration.h"
#include <iostream>
#include <cmath>
#include <time.h>
#include <thread>

/*
	Grid of cells holding number of molecules in each cell. 
	x: col-number in [0,width-1]
	y: row-number in [0,height-1]
*/


Concentration::Concentration(int width, int height)
{
	gridWidth = width;
	gridHeight = height;
	LENGTH = width*height;
	diffusionCoefficient = 1.0 / 9.0;
	grid = new std::vector<unsigned long>(LENGTH, 0);
	newGrid = new std::vector<unsigned long>(LENGTH, 0);
	std::cout << "size is " << grid->size() << std::endl;
}

Concentration::~Concentration()
{

}

void Concentration::switchGrids()
{
	temp = grid;
	grid = newGrid;
	newGrid = temp;
}

void Concentration::print()
{
	std::cout << "Current";
	for (long i = 0; i < grid->size(); i++)
	{
		if (i%gridWidth == 0)
			std::cout << std::endl;
		std::cout << grid->at(i) << ", ";
	}
	std::cout << std::endl;
	std::cout << "Total : " << total() << std::endl<<std::endl;
}

long long Concentration::total()
{
	long long sum = 0;
	for (long i = 0; i < grid->size(); i++)
	{
		sum += grid->at(i);
	}
	return sum;
}

long Concentration::linearPos(int x, int y)
{
	if (x < 0)
		x = x + gridWidth;
	if (x >= gridWidth)
		x = x % gridWidth;
	if (y < 0)
		y = y + gridHeight;
	if (y >= gridHeight)
		y = y % gridHeight;
	if (y*gridWidth + x >= LENGTH)
	{
		std::cout << "Error accessing element " << x << " " << y << " " << y*gridWidth + x << std::endl;
		return 0;
	}
	return (y*gridWidth + x);
}

std::string Concentration::getName()
{
	return "some molecule";
}

void Concentration::setCell(unsigned long value, int x, int y)
{
	setCell(value, linearPos(x, y));
}
void Concentration::setCell(unsigned long value, long pos)
{
	grid->at((pos + LENGTH) % (LENGTH)) = value;
}

unsigned long Concentration::getCell(int x, int y)
{
	return getCell(linearPos(x, y));
}
unsigned long Concentration::getCell(long pos)
{
	return grid->at((pos + (LENGTH)) % (LENGTH));
}

void Concentration::getTorusNeighbours(unsigned long * neighbours, int x, int y)
{
	neighbours[0] = (*grid)[linearPos(x, y + 1)];	//right
	neighbours[1] = (*grid)[linearPos(x + 1, y)];	//down
	neighbours[2] = (*grid)[linearPos(x, y - 1)];	//left
	neighbours[3] = (*grid)[linearPos(x - 1, y)];	//top
	neighbours[4] = (*grid)[linearPos(x - 1, y + 1)];	//topright
	neighbours[5] = (*grid)[linearPos(x + 1, y + 1)];	//downright
	neighbours[6] = (*grid)[linearPos(x + 1, y - 1)];	//downleft
	neighbours[7] = (*grid)[linearPos(x - 1, y - 1)];	//topleft
}
/*
std::vector<long> Concentration::getTorusNeighbours(long pos)
{
	pos = (pos + (LENGTH)) % (LENGTH);
	return getTorusNeighbours(pos%gridWidth, (int)((double)pos / gridWidth));
}
*/
long Concentration::getDiffSum(int x, int y)
{
	//optimize by circumventing the getTorusNeighbours call and instead summing directly
	/*
		Direct neighbours (first 4 in list) have weight 1
		Diagonal neighbours (last 4 in list) have weight 1/sqrt(2)
	*/
	//TODO: optimize this next call
	double ownValue = getCell(x, y);

	unsigned long neighbours[8];
	getTorusNeighbours(neighbours, x, y);

	double sum = 0;
	double weightedDiff = 0;
	for (int i = 0; i < 8; i++)
	{
		weightedDiff = 0;
		double diff = neighbours[i] - ownValue;
		//Don't diffuse if difference in concentration is too small
		if (abs(diff) < 2)
		{
			continue;
		}
		
		if (i < 4)
		{
			//Primary weight for orthogonal neighbours
			weightedDiff = diffusionCoefficient * primaryWeight * diff;
		}
		else
		{
			//Secondary weights for diagonal neighbours
			weightedDiff = diffusionCoefficient * secondaryWeight * diff;
		}
		//Possibly case differentitation for negative/positive weightedDiffs?
		weightedDiff = round(weightedDiff);
		sum += weightedDiff;
		/*
		if (sum != 0)
			std::cout << "sum " << sum << std::endl;
		*/
	}
	//std::cout << "total sum " << sum << std::endl;
	return (long)sum;
}
long Concentration::getDiffSum(long pos)
{
	pos = (pos + (LENGTH)) % (LENGTH);
	return getDiffSum(pos%gridWidth, (int)((double)pos / gridWidth));
}

void Concentration::tick(int t)
{
	//diffuse();
	diffuseThreaded(1);
}

void Concentration::diffuseThreaded(int num)
{
	std::vector<std::thread> threadList(num);
	for (int i = 0; i < num; i++)
	{
		int length = LENGTH / num;
		threadList[i] = std::thread(&Concentration::diffuseWorker, this, i*length, length);
	}

	for (int i = 0; i < num; i++)
	{
		threadList[i].join();
	}

	switchGrids();
}

void Concentration::diffuseWorker(int start, int length)
{
	long diff;
	long own;
	for (long i = start; i < start + length; i++)
	{
		//std::cout << "Cell NR " << i << std::endl;
		diff = getDiffSum(i);
		own = grid->at(i);
		//Carefull not to create negative concentrations
		if (diff < 0)
		{
			if (abs(diff) >= grid->at(i))
			{
				std::cout << "Error: Concentration cannot be smaller than 0." << std::endl;
				std::cout << "Position " << i << std::endl;
				newGrid->at(i) = 0;
				continue;
			}
		}
		//Overflow
		//else
		//{

		//}
		
		newGrid->at(i) = own + diff;
	}
}

void Concentration::diffuse()
{
	//Single thread version
	//clock_t start = clock();
	for (long i = 0; i < grid->size(); i++)
	{
		//std::cout << "Cell NR " << i << std::endl;
		long diff = getDiffSum(i);
		//Carefull not to create negative concentrations
		if (diff < 0)
		{
			if (abs(diff) >= grid->at(i))
			{
				std::cout << "Error: Concentration cannot be smaller than 0." << std::endl;
				std::cout << "Position " << i << std::endl;
				newGrid->at(i) = 0;
				continue;
			}
		}
		//Overflow
		else
		{
			
		}
		newGrid->at(i) = grid->at(i) + diff;
	}
	switchGrids();
	//clock_t end = clock();
	//std::cout << "Time: " << end - start << std::endl;
}

void Concentration::randomize(int number, int minimum, int maximum)
{
	if (minimum > maximum)
	{
		int a = minimum;
		minimum = maximum;
		maximum = a;
	}
	for (number; number > 0; number--)
	{
		setCell(minimum + rand() % (maximum - minimum), rand() % (LENGTH));
	}
}