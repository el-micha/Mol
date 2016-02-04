#ifndef CONCENTRATION
#define CONCENTRATION

#include <string>
#include <vector>


class Concentration
{
private:
	std::string moleculeName;
	int gridWidth;
	int gridHeight;
	std::vector<unsigned long> *grid;
	std::vector<unsigned long> *newGrid;
	std::vector<unsigned long> *temp;
	long linearPos(int, int);
	void switchGrids();
	double primaryWeight = 1;
	double secondaryWeight = 1.0 / sqrt(2);
	double diffusionCoefficient;
public:
	Concentration(int w, int h);
	~Concentration();
	std::string getName();
	void setCell(unsigned long value, int, int);
	void setCell(unsigned long value, long pos);
	unsigned long getCell(int, int);
	unsigned long getCell(long pos);
	void diffuse();
	std::vector<long> getTorusNeighbours(int, int);
	std::vector<long> getTorusNeighbours(long);
	long getDiffSum(int, int);
	long getDiffSum(long);
	long long total();
	void print();
	void randomize(int number, int minimum, int maximum);
	void tick(int t);
	void diffuseThreaded(int num);
	void diffuseWorker(int start, int length);
};

#endif