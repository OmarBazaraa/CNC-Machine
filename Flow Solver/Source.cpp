#include <iostream>
#include <ctime>
#include "Solver.h"
using namespace std;

int main(int argc, char* argv[]) {
	int startTime = clock();

	if (!init("mazes/maze10x10.txt")) {
		return 0;
	}

	printMaze();
	getPathes();
	printSolution();

	int stopTime = clock();
	cout << "time: " << (stopTime - startTime) / double(CLOCKS_PER_SEC) * 1000.0 << "ms" << endl;

	return 0;
}