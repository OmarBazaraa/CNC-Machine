#include "Solver.h"

int recursiveCalls = 0;
int dx[] = { 1, -1, 0, 0 };
int dy[] = { 0, 0, 1, -1 };
int rowsCount;
int colCounts;
int grid[25][25];	// Max 14x14 in Flow Game
vector<int> colors;
vector<pair<point, point>> colorPairs;
vector<vector<point>> colorPathCells;
vector<vector<direction>> colorPathDirections;

/* Initializes the data to be processed */
bool init(const string& directory) {
	boostIO();

	ifstream fin(directory);

	if (!fin.is_open()) {
		cout << "Cannot open the maze file!" << endl;
		return false;
	}

	fin >> rowsCount >> colCounts;

	map<int, vector<point>> mp;

	// Read the grid and store pins of the same color together
	for (int i = 0; i < rowsCount; ++i) {
		for (int j = 0; j < colCounts; ++j) {
			fin >> grid[i][j];

			if (grid[i][j] != 0) {	// if not an empty cell
				mp[grid[i][j]].push_back(point(i, j));
			}
		}
	}

	fin.close();

	// Fill in color pairs
	for (auto it : mp) {
		colors.push_back(it.first);
		colorPairs.push_back({ it.second[0], it.second[1] });
	}

	return true;
}

/* Prints the maze and a separation line after it */
void printMaze() {
	// The maze before solving
	for (int i = 0; i < rowsCount; ++i) {
		for (int j = 0; j < colCounts; ++j) {
			cout << grid[i][j] << ' ';
		}
		cout << endl;
	}

	// Adding separation between before and after solving the maze
	cout << endl;
	for (int i = colCounts * 2 + 5; i >= 0; --i) cout << "-";
	cout << endl << endl;
}

/* Prints the solution of the given maze */
void printSolution() {
	printMaze();

	cout << recursiveCalls << endl;

	for (int i = 0; i < colorPathCells.size(); ++i) {
		cout << "Color " << i + 1 << ": " << endl;

		for (int j = 0; j < colorPathDirections[i].size(); ++j) {
			switch (colorPathDirections[i][j])
			{
			case DOWN:
				cout << "v ";
				break;
			case UP:
				cout << "^ ";
				break;
			case RIGTH:
				cout << "> ";
				break;
			case LEFT:
				cout << "< ";
				break;
			}
		}
		cout << endl;

		cout << "(" << colorPathCells[i][0].r << ", " << colorPathCells[i][0].c << ")";
		for (int j = 1; j < colorPathCells[i].size(); ++j) {
			cout << "->(" << colorPathCells[i][j].r << ", " << colorPathCells[i][j].c << ")";
		}
		cout << endl << endl;
	}
}

/* Fills in the path of each given color in the grid */
void getPathes() {
	// Try solving the maze
	int r = colorPairs[0].first.r;
	int c = colorPairs[0].first.c;

	if (!solve(r, c, -1, -1, 0)) {
		// This situation should never happen in case we are running
		// Flow Game mazes as they are all solvable
		cout << "The given maze is unsolvable" << endl;
		return;
	}

	// Loop through each color to get its path
	for (int i = 0; i < colorPairs.size(); ++i) {
		vector<point> pathCells;
		vector<direction> pathDir;
		point prv(-1, -1);
		point cur = colorPairs[i].first;

		// Push the first cell into the path
		pathCells.push_back(cur);

		while (cur != colorPairs[i].second) {
			// Try the four directions
			for (int i = 0; i < 4; ++i) {
				int toR = cur.r + dx[i];
				int toC = cur.c + dy[i];

				// To prevent infinite loop
				if (prv.r == toR && prv.c == toC) {
					continue;
				}

				// If valid and matches the color then its the next cell in the path
				if (valid(toR, toC) && grid[cur.r][cur.c] == grid[toR][toC]) {
					prv = cur;
					cur.r = toR;
					cur.c = toC;
					pathDir.push_back((direction)i);
					break;
				}
			}

			// Push the current cell into the path
			pathCells.push_back(cur);
		}

		colorPathCells.push_back(pathCells);
		colorPathDirections.push_back(pathDir);
	}
}

/* Tries to solve the maze by checking all the available pathes and fills in the correct colors in the grid */
bool solve(int row, int col, int prvR, int prvC, int colorIdx) {
	++recursiveCalls;

	// Connected two pins of the same colors together
	if (row == colorPairs[colorIdx].second.r && col == colorPairs[colorIdx].second.c) {
		if (++colorIdx < colors.size()) {
			// Try to connect the next color pair
			int toX = colorPairs[colorIdx].first.r;
			int toY = colorPairs[colorIdx].first.c;
			return solve(toX, toY, -1, -1, colorIdx);
		}
		else {
			// Finished connecting every pair of colors
			return true;
		}
	}

	// Cell is already occupied so just return false
	if (row != colorPairs[colorIdx].first.r || col != colorPairs[colorIdx].first.c) {
		if (grid[row][col] != 0) {
			return false;
		}
	}

	// Try to fill the current cell with the current color
	int temp = grid[row][col];
	grid[row][col] = colors[colorIdx];

	// If the current state is unsolvable then undo the filling and no need for further branching
	if (!solvable(row, col, colorIdx)) {
		grid[row][col] = temp;
		return false;
	}

	// Try to flow in the 4 directions and check if the maze is solvable
	for (int i = 0; i < 4; ++i) {
		int toR = row + dx[i];
		int toC = col + dy[i];

		if (toR == prvR && toC == prvC) {
			continue;
		}

		if (valid(toR, toC) && solve(toR, toC, row, col, colorIdx)) {
			return true;
		}
	}

	// Undo filling the cell as the previous trial was unsolvable
	grid[row][col] = temp;

	return false;
}

/* Returns whether or not the given cell is inside the grid */
inline bool valid(int row, int col) {
	return (row >= 0 && row < rowsCount && col >= 0 && col < colCounts);
}

/* Returns whether or not the current grid state might be solvable */
inline bool solvable(int row, int col, int colorIdx) {
	// Check if one cell is isolated
	for (int i = 0; i < 4; ++i) {
		bool free = false;

		int curR = row + dx[i];
		int curC = col + dy[i];

		if (!valid(curR, curC)) {
			continue;
		}

		if (grid[curR][curC] == 0) {
			continue;
		}

		for (int j = 0; j < 4; ++j) {
			int toR = curR + dx[j];
			int toC = curC + dy[j];

			if (valid(toR, toC) && (grid[toR][toC] == 0 || grid[toR][toC] == grid[curR][curC])) {
				free = true;
				break;
			}
		}

		if (!free) {
			return false;
		}
	}

	// Check for more than one neighbour cell with the same color
	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
		int toR = row + dx[i];
		int toC = col + dy[i];

		// If its the destination cell then don't count it
		if (toR == colorPairs[colorIdx].second.r && toC == colorPairs[colorIdx].second.c) {
			continue;
		}

		if (valid(toR, toC) && grid[row][col] == grid[toR][toC]) {
			++cnt;
		}
	}
	if (cnt > 1) {
		return false;
	}

	return true;
}

/* Used to boost reading/writing from/to the console */
inline void boostIO() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
}