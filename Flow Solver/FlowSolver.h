#pragma once
// STL libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <exception>

// OpenCV libraries
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
	const int COLOR_INTENSITY_THRESHOLD = 15;
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
	int initPenRow, initPenCol;	// Needed to order color pairs in the best way for the current pen position of the robot
	int nextLevelRow, nextLevelCol;
	int recursiveCalls = 0;		// Just for statistical purposes
	vector<pair<point, point>> colorPairs;
	vector<vector<direction>> colorPathes;

public:
	/**
	 * Constructor
	 * Initial pen position is needed to get the minimum solution instructions when start solving
	 * the maze from the given cell
	 */
	FlowSolver(const string& path, int initRow = 0, int initCol = 0, bool nextLevel = false);

	/**
	 * Destructor
	 */
	~FlowSolver();

private:
	/**
	 * Loads image from the given path into a matrix of colors
	 */
	bool loadImage(const string& path);

	/**
	 * Detects the game borders and the grid size
	 */
	void detectGameStructure();

	/**
	 * Detects the location of the next level button
	 */
	void detectNextLevelButton();

	/**
	 * Fills the matrix corresponding to the image and the color pairs
	 */
	void initGameData();

	/**
	 * Compares two vectors ignoring errors according to the given threshold value.
	 * Returns true if equal, false otherwise
	 */
	bool compareVectors(const Vec3b& v1, const Vec3b& v2, int threshold);
    
	/**
	 *  Order color pairs in order to minimize the distance to switch from color to another
	 */
    void orderColorPairs(vector<pair<point, point>>& unorderedColorPairs);

public:
	/**
	 * Prints the maze and a separation line after it
	 */
	void printMaze();

	/**
	 * Prints some statistics about the maze
	 */
	void printStatistics();

	/** 
	 * Returns the solution instructions of the given maze after calling solve function
	 */
	string solutionInstructions();

	/** 
	 * Returns the instructions to go to next level button
	 */
	string goToNextLevelInstructions();

	/**
	 * Solves the game and fills in the path of each given color in the grid
	 */
	void solve();

private:
	/**
	 * Tries to solve the maze by checking all the available pathes and fills in the correct colors in the grid
	 * using depth first search (DFS) and backtracking
	 */
	bool _solve(int row, int col, int prvRow, int prvCol, int pairIdx);

	/**
	 * Returns whether or not the given cell is inside the grid
	 */
	bool valid(int row, int col);

	/** 
	 * Returns whether the current grid state might be solvable
	 */
	bool solvable(int row, int col, int colorIdx);
};