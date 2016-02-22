#ifndef CONCENTRATION
#define CONCENTRATION

#include <string>
#include <vector>
#include <random>


class Concentration
{
private:
	std::string moleculeName;
	int gridWidth;
	int gridHeight;
	long gridLength;
	long *g;
	long *ng;
	long **grid;
	long **newGrid;
	long **temp;
	long linearPos(int, int);
	void switchGrids();
	double primaryWeight = 1;
	double secondaryWeight = 1.0 / sqrt(2);
	double diffusionCoefficient;
	double secWeight;

	std::default_random_engine gen;
	std::normal_distribution<double> distro;
public:
	Concentration(int w, int h);
	~Concentration();
	std::string getName();
	void setCell(long value, int, int);
	void setCell(long value, long pos);
	long getCell(int, int);
	long getCell(long pos);
	void diffuse();
	void getTorusNeighbours(long *, long);
	long getDiffSum(long);
	long long total();
	void print();
	void randomize(int number, int minimum, int maximum);
	void tick(int t);
	void diffuseThreaded(int num);
	void diffuseWorker(int start, long length);
};

#endif