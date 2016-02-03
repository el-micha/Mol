#include "Concentration.h"
#include <iostream>
#include <cmath>
#include <time.h>

/*
	Grid of cells holding number of molecules in each cell. 
	x: col-number in [0,width-1]
	y: row-number in [0,height-1]
*/

Concentration::Concentration(int width, int height)
{
	gridWidth = width;
	gridHeight = height;
	grid = new std::vector<unsigned long>(gridHeight*gridWidth, 0);
	newGrid = new std::vector<unsigned long>(gridHeight*gridWidth, 0);
	std::cout << "size is " << grid->size() << std::endl;
}

Concentration::~Concentration()
{

}

void Concentration::switchGrids()
{
	std::vector<unsigned long> * temp = grid;
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
	x = (x + gridWidth) % gridWidth;
	y = (y + gridHeight) % gridHeight;
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
	grid->at((pos + gridWidth*gridHeight) % (gridWidth*gridHeight)) = value;
}

unsigned long Concentration::getCell(int x, int y)
{
	return getCell(linearPos(x, y));
}
unsigned long Concentration::getCell(long pos)
{
	return grid->at((pos + (gridWidth*gridHeight)) % (gridWidth*gridHeight));
}

std::vector<long> Concentration::getTorusNeighbours(int x, int y)
{
	std::vector<long> neighbours(0);
	neighbours.push_back(linearPos(x-1, y));	//top
	neighbours.push_back(linearPos(x, y+1));	//right
	neighbours.push_back(linearPos(x+1, y));	//down
	neighbours.push_back(linearPos(x, y-1));	//left
	neighbours.push_back(linearPos(x-1, y+1));	//topright
	neighbours.push_back(linearPos(x+1, y+1));	//downright
	neighbours.push_back(linearPos(x+1, y-1));	//downleft
	neighbours.push_back(linearPos(x-1, y-1));	//upleft

	return neighbours;
}

std::vector<long> Concentration::getTorusNeighbours(long pos)
{
	pos = (pos + (gridWidth*gridHeight)) % (gridWidth*gridHeight);
	return getTorusNeighbours(pos%gridWidth, (int)((double)pos / gridWidth));
}

long Concentration::getDiffSum(int x, int y)
{
	//optimize by circumventing the getTorusNeighbours call and instead summing directly
	/*
		Direct neighbours (first 4 in list) have weight 1
		Diagonal neighbours (last 4 in list) have weight 1/sqrt(2)
	*/
	double ownValue = getCell(x, y);
	std::vector<long> neighbours = getTorusNeighbours(x, y);
	double sum = 0;
	double weight = 1.0;
	double diffusionCoeff = 1.0 / 9.0;
	for (int i = 0; i < neighbours.size(); i++)
	{
		double diff = getCell(neighbours.at(i)) - ownValue;
		//Don't diffuse if difference in concentration is too small
		if (abs(diff) < 2)
		{
			continue;
		}
		
		if (i > 3)
			weight = 1 / sqrt(2);
		/*
		if (ownValue != 0)
			std::cout << "ownvalue " << ownValue << std::endl;
		if (getCell(neighbours.at(i)) != 0)
			std::cout << "neighbour " << getCell(neighbours.at(i)) << std::endl;
		*/
		double weightedDiff = diffusionCoeff * weight * diff;
		if (weightedDiff < 0)
		{
			weightedDiff = round(weightedDiff);
		}
		else
		{
			weightedDiff = round(weightedDiff);
		}
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
	pos = (pos + (gridWidth*gridHeight)) % (gridWidth*gridHeight);
	return getDiffSum(pos%gridWidth, (int)((double)pos / gridWidth));
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

//Git test