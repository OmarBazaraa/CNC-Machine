#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include "FlowSolver.h"
using namespace std;

#define IMAGE_FILE          "mazes/maze8x8.png"
#define INSTRUCTIONS_FILE   "instructions.txt"

/**
 * Used to boost reading/writing from/to the console
 */
inline void boostIO() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
}

inline string solveMaze(FlowSolver& solver);
inline string goToNextLevel(FlowSolver& solver);

/**
 * Main function
 */
int main(int argc, char* argv[]) {
	boostIO();

	int startTime = clock();

	int initRow = 0;
	int initCol = 0;
	bool nextLevel = false;

	if (argc >= 3) {
		initRow = (int)argv[1];
		initCol = (int)argv[2];
	}

	if (argc >= 4) {
		nextLevel = (bool)argv[3];
	}

	try {
		FlowSolver solver(IMAGE_FILE, initRow, initCol, nextLevel);
		string instructions = (nextLevel ? goToNextLevel(solver) : solveMaze(solver));
		cout << "Instructions: " << endl << instructions << endl;

		// Output instructions into a file to be send to Arduino
		ofstream fout(INSTRUCTIONS_FILE);
		fout << instructions;
		fout.close();
	}
	catch (const exception& ex) {
		cout << "ERROR::" << ex.what() << endl;
	}

	int stopTime = clock();
	cout << endl << "Time: " << (stopTime - startTime) / double(CLOCKS_PER_SEC) * 1000.0 << "ms" << endl;

	return 0;
}

inline string solveMaze(FlowSolver& solver) {
	// Print initial maze
	solver.printMaze();

	// Solve the maze
	solver.solve();

	// Print the maze and some statistics after solving it
	solver.printMaze();
	solver.printStatistics();

	// Return instructions
	return solver.solutionInstructions();
}

inline string goToNextLevel(FlowSolver& solver) {
	// Return instructions
	return solver.goToNextLevelInstructions();
}