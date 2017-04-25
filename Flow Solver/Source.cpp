#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include "FlowSolver.h"
using namespace std;

/**
 * Used to boost reading/writing from/to the console
 */
inline void boostIO() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
}

/**
 * Main function
 */
int main() {
	boostIO();

	int startTime = clock();

	try {
		FlowSolver solver("mazes/game.png");
		solver.printMaze();
		solver.solve();
		string solution = solver.getSolutionPaths();
		cout << "Instructions: " << solution << endl;
		ofstream fout("Path.txt");
		fout << solution;
		fout.close();
	}
	catch (const exception& ex) {
		cout << "ERROR::" << ex.what() << endl;
	}

	int stopTime = clock();
	cout << endl << "Time: " << (stopTime - startTime) / double(CLOCKS_PER_SEC) * 1000.0 << "ms" << endl;

	return 0;
}