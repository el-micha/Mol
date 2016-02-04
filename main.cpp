#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "Concentration.h"
#include "Sim.h"



int main(int argc, char* argv[])
{
	Sim simulation = Sim(64, 32);

	simulation.run();

	/*
	Concentration mol = Concentration(24, 12);
	mol.setCell(4000, 16, 8);
	mol.print();
	mol.diffuse();
	mol.print();
	mol.diffuse();
	mol.print();
	mol.diffuse();
	mol.print();
	mol.diffuse();
	mol.print();
	mol.diffuse();
	mol.print();
	mol.diffuse();
	mol.print();
	*/
	
	return 0;
}