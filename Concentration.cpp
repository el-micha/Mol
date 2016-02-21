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
	gridLength = width*height;
	diffusionCoefficient = 1.0 / 9.0;
	//grid = new std::vector<unsigned long>(gridLength, 0);
	//newGrid = new std::vector<unsigned long>(gridLength, 0);
	g = new unsigned long[gridLength];
	ng = new unsigned long[gridLength];
	for (long i = 0; i < gridLength; i++)
	{
		g[i] = 0;
		ng[i] = 0;
	}
	grid = &g;
	newGrid = &ng;
	std::cout << "size is " << gridLength << std::endl;
}

Concentration::~Concentration()
{
	delete g;
	delete ng;
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
	for (long i = 0; i < gridLength; i++)
	{
		if (i%gridWidth == 0)
			std::cout << std::endl;
		std::cout << (*grid)[i] << ", ";
	}
	std::cout << std::endl;
	std::cout << "Total : " << total() << std::endl<<std::endl;
}

long long Concentration::total()
{
	long long sum = 0;
	for (long i = 0; i < gridLength; i++)
	{
		sum += (*grid)[i];
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
		
	//x = (x + gridWidth) % gridWidth;
	//y = (y + gridHeight) % gridHeight;

	if (y*gridWidth + x >= gridLength)
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
	(*grid)[(pos + gridLength) % (gridLength)] = value;
}

unsigned long Concentration::getCell(int x, int y)
{
	return getCell(linearPos(x, y));
}
unsigned long Concentration::getCell(long pos)
{
	return (*grid)[(pos + (gridLength)) % (gridLength)];
}

void Concentration::getTorusNeighbours(unsigned long * neighbours, long pos)
{
	//edge cases with ifs or with modulos: what's more efficient?

	neighbours[0] = (*grid)[((pos + 1) + gridLength) % gridLength];	//right
	neighbours[1] = (*grid)[((pos - 1) + gridLength) % gridLength];	//left
	neighbours[2] = (*grid)[((pos + gridWidth) + gridLength) % gridLength];	//down
	neighbours[3] = (*grid)[((pos + gridWidth + 1) + gridLength) % gridLength];	//downright
	neighbours[4] = (*grid)[((pos + gridWidth - 1) + gridLength) % gridLength];	//downleft
	neighbours[5] = (*grid)[((pos - gridWidth) + gridLength) % gridLength];	//top
	neighbours[6] = (*grid)[((pos - gridWidth + 1) + gridLength) % gridLength];	//topright
	neighbours[7] = (*grid)[((pos - gridWidth - 1) + gridLength) % gridLength];	//topleft
}

long Concentration::getDiffSum(long pos)
{
	double ownValue = getCell(pos);

	unsigned long neighbours[8];
	getTorusNeighbours(neighbours, pos);

	double sum = 0;
	
	sum += (neighbours[0] - ownValue) * diffusionCoefficient * primaryWeight;
	sum += (neighbours[1] - ownValue) * diffusionCoefficient * primaryWeight;
	sum += (neighbours[2] - ownValue) * diffusionCoefficient * primaryWeight;
	sum += (neighbours[3] - ownValue) * diffusionCoefficient * primaryWeight;
	sum += (neighbours[4] - ownValue) * diffusionCoefficient * secondaryWeight;
	sum += (neighbours[5] - ownValue) * diffusionCoefficient * secondaryWeight;
	sum += (neighbours[6] - ownValue) * diffusionCoefficient * secondaryWeight;
	sum += (neighbours[7] - ownValue) * diffusionCoefficient * secondaryWeight;

	return (long)sum;
}

void Concentration::tick(int t)
{
	diffuseThreaded(1);
}

void Concentration::diffuseThreaded(int num)
{
	std::vector<std::thread> threadList(num);
	for (int i = 0; i < num; i++)
	{
		int len = gridLength / num;
		threadList[i] = std::thread(&Concentration::diffuseWorker, this, i*len, len);
	}

	for (int i = 0; i < num; i++)
	{
		threadList[i].join();
	}

	switchGrids();
}

void Concentration::diffuseWorker(int start, long gridLength)
{
	long diff;
	long own;
	for (long i = start; i < start + gridLength; i++)
	{
		diff = getDiffSum(i);
		own = (*grid)[i];
		//Carefull not to create negative concentrations
		if (diff < 0)
		{
			if (abs(diff) >= (*grid)[i])
			{
				std::cout << "Error: Concentration cannot be smaller than 0." << std::endl;
				std::cout << "Position " << i << std::endl;
				(*newGrid)[i] = 0;
				continue;
			}
		}
		//Overflow
		//else
		//{

		//}
		
		(*newGrid)[i] = own + diff;
	}
}

void Concentration::diffuse()
{
	//Single thread version
	//clock_t start = clock();
	for (long i = 0; i < gridLength; i++)
	{
		//std::cout << "Cell NR " << i << std::endl;
		long diff = getDiffSum(i);
		//Carefull not to create negative concentrations
		if (diff < 0)
		{
			if (abs(diff) >= (*grid)[i])
			{
				std::cout << "Error: Concentration cannot be smaller than 0." << std::endl;
				std::cout << "Position " << i << std::endl;
				(*newGrid)[i] = 0;
				continue;
			}
		}
		//Overflow
		else
		{
			
		}
		(*newGrid)[i] = (*grid)[i] + diff;
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
		setCell(minimum + rand() % (maximum - minimum), (long)((gridLength / 32768) * rand() + rand()) % (gridLength));
	}
}