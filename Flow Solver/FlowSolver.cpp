#include "FlowSolver.h"

FlowSolver::FlowSolver(const string& url) {
	// Load the image into matrix image
	if (!loadImage(url)) {
		string errorMessage = "Could not load the image at: " + url;
		throw exception(errorMessage.c_str());
	}

	detectGameStructure();
	initGameData();
}

FlowSolver::~FlowSolver() {

}

bool FlowSolver::loadImage(const string& url) {
	image = imread(url, CV_LOAD_IMAGE_COLOR);   // Read the file

	if (!image.data) { // Check for invalid input
		return false;
	}

	return true;
}

void FlowSolver::detectGameStructure() {
	gridColsCount = gridRowsCount = 0;
	singleBlockWidth = singleBlockHeight = -1;
	horizontalBorderThickness = verticalBorderThickness = -1;
	leftBorder = topBorder = bottomBorder = -1;
	
	Vec3b borderIntensity;
	int imageHalfRowsCount = image.rows / 2;
	int imageHalfColsCount = image.cols / 2;

	// Loop to get border color, left border and vertial border thickness
	for (int i = 1; i < image.cols; ++i) {
		if (image.at<Vec3b>(imageHalfRowsCount, i) != image.at<Vec3b>(imageHalfRowsCount, i - 1)) {
			if (leftBorder == -1) {
				borderIntensity = image.at<Vec3b>(imageHalfRowsCount, i);
				leftBorder = i;
			}
			else {
				verticalBorderThickness = i - leftBorder;
				break;
			}
		}
	}
	// Loop to get cell width
	for (int i = leftBorder + verticalBorderThickness; i < image.cols; ++i) {
		// @TODO: What if imageHalfRowsCount stands on a horizontal border
		if (image.at<Vec3b>(imageHalfRowsCount, i) == borderIntensity) {
			singleBlockWidth = i - (leftBorder + verticalBorderThickness);
			break;
		}
	}
	// Loop to get number of columns in the grid
	for (int i = (leftBorder + verticalBorderThickness + singleBlockWidth / 2); i < image.cols; i += verticalBorderThickness + singleBlockWidth) {
		++gridColsCount;
	}

	// Loop from top until top border to get top border and top border thickness
	for (int i = 0; i < image.rows; ++i) {
		Vec3b intensity = image.at<Vec3b>(i, leftBorder + horizontalBorderThickness + 10);
		if (intensity == borderIntensity && topBorder == -1) {
			topBorder = i;
		}
		else if (intensity != borderIntensity && topBorder != -1) {
			horizontalBorderThickness = i - topBorder;
			break;
		}
	}
	// Loop to get bottom border
	for (int i = image.rows - 1; i > 0; --i) {
		if (image.at<Vec3b>(i, imageHalfColsCount) == borderIntensity) {
			bottomBorder = i;
			break;
		}
	}
	// Loop to get cell height
	for (int i = topBorder + horizontalBorderThickness; i < image.rows; ++i) {
		if (image.at<Vec3b>(i, leftBorder + horizontalBorderThickness + 2) == borderIntensity) {
			singleBlockHeight = i - (topBorder + horizontalBorderThickness);
			break;
		}
	}
	// Loop to get number of rows in the grid
	for (int i = (topBorder + horizontalBorderThickness + singleBlockHeight / 2); i < bottomBorder; i += horizontalBorderThickness + singleBlockHeight) {
		++gridRowsCount;
	}
}

void FlowSolver::initGameData() {
	int currentColorID = 0;	// counter used for setting matrix cells colors numbers
	map<colorRGB, int> colorsIDs;

	for (int i = 0; i < gridRowsCount; ++i) { // i for y
		for (int j = 0; j < gridColsCount; ++j) { // j for x
			Vec3b intensity = image.at<Vec3b>(
				(topBorder + horizontalBorderThickness * (i + 1) + singleBlockHeight * (i + 0.5)),
				(leftBorder + verticalBorderThickness * (j + 1) + singleBlockWidth * (j + 0.5))
			);

			if (((int)intensity.val[0] + (int)intensity.val[1] + (int)intensity.val[2]) > MAX_BACKGROUND_RGB) {
				colorRGB cellIntesity(
					(int)intensity.val[2],
					(int)intensity.val[1],
					(int)intensity.val[0]
				); // BGR -> RGB

				if (colorsIDs[cellIntesity] > 0) {
					grid[i][j] = colorsIDs[cellIntesity];
					colorPairs[grid[i][j] - 1].second = point(i, j);
				}
				else {
					grid[i][j] = colorsIDs[cellIntesity] = ++currentColorID;
					colorPairs.push_back({ point(i, j), point() });
				}
			}
		}
	}
}

void FlowSolver::printMaze() {
	cout << gridRowsCount << "x" << gridColsCount << endl;
	for (int i = 0; i < gridRowsCount; ++i) {
		for (int j = 0; j < gridColsCount; ++j) {
			cout << setw(5) << grid[i][j];
		}
		cout << endl;
	}

	// Adding separation between before and after solving the maze
	cout << endl;
	for (int i = gridColsCount * 2 + 5; i >= 0; --i) cout << "-";
	cout << endl << endl;
}

void FlowSolver::printSolution() {
	printMaze();

	cout << "Recursive calls count: " << recursiveCalls << endl;
	cout << "Block Width: " << singleBlockWidth << ", Block Height: " << singleBlockHeight << endl;

	for (int i = 0; i < colorPathes.size(); ++i) {
		cout << "Color " << i + 1 << ": " << endl;
		cout << "From (" << colorPairs[i].first.r << ", " << colorPairs[i].first.c << "): ";
		for (int j = 0; j < colorPathes[i].size(); ++j) {
			switch (colorPathes[i][j])
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
	}
}

void FlowSolver::solve() {
	// Try solving the maze
	int r = colorPairs[0].first.r;
	int c = colorPairs[0].first.c;

	if (!_solve(r, c, -1, -1, 0)) {
		// This situation should never happen in case we are running
		// Flow Game mazes as they are all solvable
		std::cout << "The given maze is unsolvable" << std::endl;
		return;
	}

	// Loop through each color to get its path
	for (int i = 0; i < colorPairs.size(); ++i) {
		vector<direction> path;
		point prv(-1, -1);
		point cur = colorPairs[i].first;

		while (cur != colorPairs[i].second) {
			// Try the four directions
			for (int i = 0; i < 4; ++i) {
				int toR = cur.r + dx[i];
				int toC = cur.c + dy[i];

				// If valid and matches the color then its the next cell in the path
				if ((toR != prv.r || toC != prv.c) && valid(toR, toC) && grid[cur.r][cur.c] == grid[toR][toC]) {
					prv = cur;
					cur.r = toR;
					cur.c = toC;
					path.push_back((direction)i);
					break;
				}
			}
		}

		colorPathes.push_back(path);
	}
}

bool FlowSolver::_solve(int row, int col, int prvRow, int prvCol, int colorIdx) {
	++recursiveCalls;

	// Connected two pins of the same colors together
	if (row == colorPairs[colorIdx].second.r && col == colorPairs[colorIdx].second.c) {
		if (++colorIdx < colorPairs.size()) {
			// Try to connect the next color pair
			int toX = colorPairs[colorIdx].first.r;
			int toY = colorPairs[colorIdx].first.c;
			return _solve(toX, toY, -1, -1, colorIdx);
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
	grid[row][col] = colorIdx + 1;

	// If the current state is unsolvable then undo the filling and no need for further branching
	if (!solvable(row, col, colorIdx)) {
		grid[row][col] = temp;
		return false;
	}

	// Try to flow in the 4 directions and check if the maze is solvable
	for (int i = 0; i < 4; ++i) {
		int toR = row + dx[i];
		int toC = col + dy[i];

		if ((toR != prvRow || toC != prvCol) && valid(toR, toC) && _solve(toR, toC, row, col, colorIdx)) {
			return true;
		}
	}

	// Undo filling the cell as the previous trial was unsolvable
	grid[row][col] = temp;

	return false;
}

bool FlowSolver::valid(int row, int col) {
	return (row >= 0 && row < gridRowsCount && col >= 0 && col < gridColsCount);
}

bool FlowSolver::solvable(int row, int col, int colorIdx) {
	// Check if one cell is isolated
	for (int i = 0; i < 4; ++i) {
		bool free = false;

		int curR = row + dx[i];
		int curC = col + dy[i];

		if (!valid(curR, curC) || grid[curR][curC] == EMPTY_BLOCK_COLOR) {
			continue;
		}

		for (int j = 0; j < 4; ++j) {
			int toR = curR + dx[j];
			int toC = curC + dy[j];

			if (valid(toR, toC) && (grid[toR][toC] == EMPTY_BLOCK_COLOR || grid[toR][toC] == grid[curR][curC])) {
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