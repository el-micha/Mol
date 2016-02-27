#ifndef SOLUTION
#define SOLUTION

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
	void randomize(long intensity);
};

#endif