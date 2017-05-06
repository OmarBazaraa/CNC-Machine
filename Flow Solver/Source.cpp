#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include "FlowSolver.h"
using namespace std;

#define IMAGE_FILE          "mazes/next.png"
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
inline void outputInstructions(const string& instructions, const string& path);

/**
 * Main function
 */
int main(int argc, char* argv[]) {
	boostIO();

	int startTime = clock();

	// Get passed arguments
	string imagePath = IMAGE_FILE;
	string instructionsPath = INSTRUCTIONS_FILE;
	int initRow = 4;
	int initCol = 4;
	bool nextLevel = true;

	// Get image path
	if (argc >= 2) {
		imagePath = argv[1];
	}
	// Get instructions path
	if (argc >= 3) {
		instructionsPath = argv[2];
	}
	// Get intial position
	if (argc >= 5) {
		initRow = atoi(argv[3]);
		initCol = atoi(argv[4]);
	}
	// Get mode of operation
	if (argc >= 6) {
		nextLevel = (argv[5] != "0" && argv[5] != "false");
	}

	try {
		FlowSolver solver(imagePath, initRow, initCol, nextLevel);
		string instructions = (nextLevel ? goToNextLevel(solver) : solveMaze(solver));
		cout << "Instructions: " << endl << instructions << endl;
		outputInstructions(instructions, instructionsPath);
	}
	catch (const exception& ex) {
		outputInstructions("-1", instructionsPath);
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

inline void outputInstructions(const string& instructions, const string& path) {
	// Output instructions into a file to be send to Arduino
	ofstream fout(path);
	fout << instructions;
	fout.close();
}