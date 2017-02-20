#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

/*
	Enum having all possible directions in Flow Game
	Note that they are in the same order of "dx" and "dy" arrays below
*/
enum direction {
	DOWN,
	UP,
	RIGTH,
	LEFT
};

/* Structure to hold the coordinates of a cell */
struct point {
	int r;	// Row
	int c;	// Column

	inline point(int i, int j) {
		r = i;
		c = j;
	}

	inline bool operator==(const point& rhs) const {
		return (r == rhs.r && c == rhs.c);
	}

	inline bool operator!=(const point& rhs) const {
		return (r != rhs.r || c != rhs.c);
	}
};

/* Initializes the data to be processed */
bool init(const string& directory);

/* Prints the maze and a separation line after it */
void printMaze();

/* Prints the solution of the given maze */
void printSolution();

/* Fills in the path of each given color in the grid */
void getPathes();

/* Tries to solve the maze by checking all the available pathes and fills in the correct colors in the grid */
bool solve(int row, int col, int prvR, int prvC, int color_idx);

/* Returns whether or not the given cell is inside the grid */
inline bool valid(int row, int col);

/* Returns whether or not the current grid state might be solvable */
inline bool solvable(int row, int col, int colorIdx);

/* Used to boost reading/writing from/to the console */
inline void boostIO();