#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "Concentration.h"
#include "Sim.h"



int main(int argc, char* argv[])
{
	
	Sim simulation = Sim(512, 256);
	simulation.run();
	
	return 0;
}