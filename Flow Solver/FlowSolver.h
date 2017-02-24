#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

class FlowSolver
{
private:
	/**
	 * Enum having all possible directions in Flow Game
	 * Note that they are in the same order of "dx" and "dy" arrays below
	 */
	enum direction {
		DOWN,
		UP,
		RIGTH,
		LEFT
	};

	/**
	 * Structure to hold the coordinates of a cell
	 */
	struct point {
		int r;	// Row
		int c;	// Column

		point() {
			r = c = 0;
		}

		point(int i, int j) {
			r = i;
			c = j;
		}

		bool operator==(const point& rhs) const {
			return (r == rhs.r && c == rhs.c);
		}

		bool operator!=(const point& rhs) const {
			return (r != rhs.r || c != rhs.c);
		}
	};

	/**
	 * Structure to hold color intensity in RGB format 
	 */
	struct colorRGB {
		pair<int, pair<int, int>> vec;

		colorRGB() {

		}

		colorRGB(int r, int g, int b) {
			vec.first = r;
			vec.second.first = g;
			vec.second.second = b;
		}

		bool operator<(const colorRGB& rhs) const {
			return vec < rhs.vec;
		}
	};

private:
	const int EMPTY_BLOCK_COLOR = 0;
	const int MAX_BACKGROUND_RGB = 100;
	const int dx[4] = { 1, -1, 0, 0 };
	const int dy[4] = { 0, 0, 1, -1 };

private:
	Mat image;
	int grid[25][25];			// Max 14x14 in Flow Game
	int gridRowsCount, gridColsCount;
	int singleBlockWidth, singleBlockHeight;
	int horizontalBorderThickness, verticalBorderThickness;
	int leftBorder, topBorder, bottomBorder;
	int recursiveCalls = 0;		// Just for statistical purposes
	vector<pair<point, point>> colorPairs;
	vector<vector<direction>> colorPathes;

public:
	/**
	 * Constructor
	 */
	FlowSolver(const string& url);

	/**
	 * Destructor
	 */
	~FlowSolver();

private:
	/**
	 * Loads image from the given url into a matrix of colors
	 */
	bool loadImage(const string& url);

	/**
	 * This function detects the game borders and the grid size
	 */
	void detectGameStructure();

	/**
	 * This function fills the matrix corresponding to the image and the color pairs
	 */
	void initGameData();

public:
	/**
	 * Prints the maze and a separation line after it
	 */
	void printMaze();

	/** 
	 * Prints the solution of the given maze
	 */
	void printSolution();

	/**
	 * Solves the game and fills in the path of each given color in the grid
	 */
	void solve();

private:
	/**
	 * Tries to solve the maze by checking all the available pathes and fills in the correct colors in the grid
	 * using depth first search (DFS) and backtracking
	 */
	bool _solve(int row, int col, int prvRow, int prvCol, int colorIdx);

	/**
	 * Returns whether or not the given cell is inside the grid
	 */
	bool valid(int row, int col);

	/** 
	 * Returns whether the current grid state might be solvable
	 */
	bool solvable(int row, int col, int colorIdx);
};