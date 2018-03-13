#include "Concentration.h"
#include <iostream>
#include <cmath>
#include <time.h>
#include <thread>
#include <random>

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
	secWeight = 1.0 / sqrt(2);
	g = new long[gridLength];
	ng = new long[gridLength];
	for (long i = 0; i < gridLength; i++)
	{
		g[i] = 0;
		ng[i] = 0;
	}
	grid = &g;
	newGrid = &ng;
	std::cout << "size is " << gridLength << std::endl;

	gen = std::default_random_engine();
	distro = std::normal_distribution<double>(1,1);
};

Concentration::~Concentration()
{
	delete g;
	delete ng;
}

void Concentration::switchGrids()
{
	/*
	temp = grid;
	grid = newGrid;
	newGrid = temp;
	*/
	clock_t s = clock();
	for (long i = 0; i < gridLength; i++)
	{
		(*grid)[i] = (*newGrid)[i];
	}
	//std::cout << "copy time " << s - clock() << std::endl;
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
	std::cout << "New";
	for (long i = 0; i < gridLength; i++)
	{
		if (i%gridWidth == 0)
			std::cout << std::endl;
		std::cout << (*newGrid)[i] << ", ";
	}
	std::cout << std::endl;
	//std::cout << "Total : " << total() << std::endl<<std::endl;
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

void Concentration::setCell(long value, int x, int y)
{
	setCell(value, linearPos(x, y));
}
void Concentration::setCell(long value, long pos)
{
	(*grid)[(pos + gridLength) % (gridLength)] = value;
	(*newGrid)[(pos + gridLength) % (gridLength)] = value;
}

long Concentration::getCell(int x, int y)
{
	return getCell(linearPos(x, y));
}
long Concentration::getCell(long pos)
{
	return (*grid)[(pos + (gridLength)) % (gridLength)];
}
/*
void Concentration::getTorusNeighbours(long * neighbours, long pos)
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
*/
long Concentration::getDiffSum(long pos)
{
	double ownValue = getCell(pos);

	long neighbours[8];
	//getTorusNeighbours(neighbours, pos);
	neighbours[0] = (*grid)[((pos + 1) + gridLength) % gridLength];	//right
	neighbours[1] = (*grid)[((pos - 1) + gridLength) % gridLength];	//left
	neighbours[2] = (*grid)[((pos + gridWidth) + gridLength) % gridLength];	//down
	neighbours[3] = (*grid)[((pos + gridWidth + 1) + gridLength) % gridLength];	//downright
	neighbours[4] = (*grid)[((pos + gridWidth - 1) + gridLength) % gridLength];	//downleft
	neighbours[5] = (*grid)[((pos - gridWidth) + gridLength) % gridLength];	//top
	neighbours[6] = (*grid)[((pos - gridWidth + 1) + gridLength) % gridLength];	//topright
	neighbours[7] = (*grid)[((pos - gridWidth - 1) + gridLength) % gridLength];	//topleft

	double sum = 0;
	
	sum += (neighbours[0] - ownValue) * diffusionCoefficient * primaryWeight; 
	sum += (neighbours[1] - ownValue) * diffusionCoefficient * primaryWeight; 
	sum += (neighbours[2] - ownValue) * diffusionCoefficient * primaryWeight; 
	sum += (neighbours[3] - ownValue) * diffusionCoefficient * secondaryWeight; //
	sum += (neighbours[4] - ownValue) * diffusionCoefficient * secondaryWeight; //
	sum += (neighbours[5] - ownValue) * diffusionCoefficient * primaryWeight; 
	sum += (neighbours[6] - ownValue) * diffusionCoefficient * secondaryWeight; //
	sum += (neighbours[7] - ownValue) * diffusionCoefficient * secondaryWeight; //

	
	return (long)sum;
}

void Concentration::diffuseWorker(int start, long len)
{
	long own;
	long diffSum;
	
	long topRight;
	long right;
	long downRight;
	long down;

	long diffTopright;
	long diffRight;
	long diffDownright;
	long diffDown;

	int dir = 0;
	int repetitions = 1;	//1 definitely looks best
	
	//IMPORTANT:
	//apparently, each thread needs to seed its RNG, otherwise very weird stuff happens with brownian motion.
	srand((int)clock());

	//std::default_random_engine gen;
	//std::normal_distribution<double> distro(0, 1);

	for (long i = start; i < start + len; i++)
	{
		own = (*grid)[i];

		topRight = (*grid)[((i - gridWidth + 1) + gridLength) % gridLength];
		right =  (*grid)[((i + 1) + gridLength) % gridLength];
		downRight = (*grid)[((i + gridWidth + 1) + gridLength) % gridLength];
		down = (*grid)[((i + gridWidth) + gridLength) % gridLength];

		diffTopright = diffusionCoefficient * secWeight * (topRight - own); // * distro(gen)
		diffRight = diffusionCoefficient * (right - own);
		diffDownright = diffusionCoefficient * secWeight * (downRight - own);
		diffDown = diffusionCoefficient * (down - own);


		diffSum = diffTopright + diffRight + diffDownright + diffDown;

		//std::cout << "own " << own << " topright " << diffTopright << " right " << diffRight << " downRight " << diffDownright << " down " << diffDown << std::endl;
		//std::cout << "Nown " << (*newGrid)[i] << " Ntopright " << (*newGrid)[((i - gridWidth + 1) + gridLength) % gridLength] << " Nright " << (*newGrid)[((i + 1) + gridLength) % gridLength] << " NdownRight " << (*newGrid)[((i + gridWidth + 1) + gridLength) % gridLength] << " Ndown " << (*newGrid)[((i + gridWidth) + gridLength) % gridLength] << std::endl<<std::endl;

		(*newGrid)[i] += diffSum;

		(*newGrid)[((i - gridWidth + 1) + gridLength) % gridLength] -= diffTopright;
		(*newGrid)[((i + 1) + gridLength) % gridLength] -= diffRight;
		(*newGrid)[((i + gridWidth + 1) + gridLength) % gridLength] -= diffDownright;
		(*newGrid)[((i + gridWidth) + gridLength) % gridLength] -= diffDown;


		//if new content greater than 0, randomly diffuse rep times
		
		for (int r = 0; r < repetitions; r++)
		{
			//if (rand() % 10 > 0)
			//	continue;
			
			//fast but bad random:
			int diff = rand();
			if (diff < RAND_MAX / 2)
				diff = 1;
			else
				diff = -1;

			//slow but good random:
			//if (start > 0)	
			//	diff = round(distro(gen));

			if ((*newGrid)[i] - diff >= 0)
			{
				//dir = (int)(distro(gen)*100) % 4;
				/*
				dir = rand();
				if (dir < 1.0 / 4.0 * RAND_MAX)
					dir = 3;
				if (dir > 1.0 / 4.0 * RAND_MAX)
					dir = 2;
				if (dir > 2.0 / 4.0 * RAND_MAX)
					dir = 1;
				if (dir > 3.0 / 4.0 * RAND_MAX)
					dir = 0;
				*/
				dir = (int)(rand() / 10) % 4;

				if (dir == 0)
				{
					if ((*newGrid)[((i - gridWidth + 1) + gridLength) % gridLength] + diff >= 0)
					{
						(*newGrid)[i] -= diff;
						(*newGrid)[((i - gridWidth + 1) + gridLength) % gridLength] += diff;
					}
				}
				else if (dir == 1)
				{
					if ((*newGrid)[((i + 1) + gridLength) % gridLength] + diff >= 0)
					{
						(*newGrid)[i] -= diff;
						(*newGrid)[((i + 1) + gridLength) % gridLength] += diff;
					}
				}
				else if (dir == 2)
				{
					if ((*newGrid)[((i + gridWidth + 1) + gridLength) % gridLength] + diff >= 0)
					{
						(*newGrid)[i] -= diff;
						(*newGrid)[((i + gridWidth + 1) + gridLength) % gridLength] += diff;
					}
				}
				else if (dir == 3)
				{
					if ((*newGrid)[((i + gridWidth) + gridLength) % gridLength] + diff >= 0)
					{
						(*newGrid)[i] -= diff;
						(*newGrid)[((i + gridWidth) + gridLength) % gridLength] += diff;
					}
				}
			}
		}
		

		//print();

		//TODO: Handle Overflow
	}
}


void Concentration::tick(int t)
{
	diffuseThreaded(3);
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