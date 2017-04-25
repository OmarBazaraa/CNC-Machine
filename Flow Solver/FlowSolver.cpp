#include "FlowSolver.h"

FlowSolver::FlowSolver(const string& url) {
    // Load the image into matrix image
    if (!loadImage(url)) {
        string errorMessage = "Could not load the image at: " + url;
        cout << errorMessage<<endl;
        
        return;
        //throw exception(errorMessage.c_str());
    }
    
    detectGameStructure();
    initGameData();
}

FlowSolver::~FlowSolver() {
	image.deallocate();
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
    
    // Loop to get border color and leftBorder position
    for (int i = 1; i < image.cols; ++i) {
        if (image.at<Vec3b>(imageHalfRowsCount, i) != image.at<Vec3b>(imageHalfRowsCount, i - 1)) {
            borderIntensity = image.at<Vec3b>(imageHalfRowsCount, i);
            leftBorder = i;
            break;
        }
    }
    // Loop from top until top border to get top border position
    for (int i = 0; i < image.rows; ++i) {
        Vec3b intensity = image.at<Vec3b>(i, imageHalfColsCount);
        if (intensity == borderIntensity) {
            topBorder = i;
            break;
        }
    }
    // Loop to get vertial border thickness
    for (int i = leftBorder; i < image.cols; ++i) {
        if (image.at<Vec3b>(topBorder + 10, i) != borderIntensity) {
            verticalBorderThickness = i - leftBorder;
            break;
        }
    }
    // Loop to get horizontal border thickness
    for (int i = topBorder; i < image.rows; ++i) {
        Vec3b intensity = image.at<Vec3b>(i, leftBorder + verticalBorderThickness + 10);
        if (intensity != borderIntensity) {
            horizontalBorderThickness = i - topBorder;
            break;
        }
    }
    // Loop to get bottom border
    for (int i = image.rows - 1; i > 0; --i) {
        if (image.at<Vec3b>(i, leftBorder + verticalBorderThickness + 10) == borderIntensity) {
            bottomBorder = i;
            break;
        }
    }
    
    // Loop to get cell width
    for (int i = leftBorder + verticalBorderThickness; i < image.cols; ++i) {
        if (image.at<Vec3b>(topBorder + horizontalBorderThickness + 10, i) == borderIntensity) {
            singleBlockWidth = i - (leftBorder + verticalBorderThickness);
            break;
        }
    }
    // Loop to get number of columns in the grid
    for (int i = (leftBorder + verticalBorderThickness + singleBlockWidth / 2); i < image.cols; i += verticalBorderThickness + singleBlockWidth) {
        ++gridColsCount;
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
    vector<pair<point, point>> unorderedColorPairs;
    
    for (int i = 0; i < gridRowsCount; ++i) { // i for y
        for (int j = 0; j < gridColsCount; ++j) { // j for x
            Vec3b intensity = image.at<Vec3b>(
				(topBorder + horizontalBorderThickness * (i + 1) + singleBlockHeight * (i + 0.5)),
				(leftBorder + verticalBorderThickness * (j + 1) + singleBlockWidth * (j + 0.5))
			);
            
            if (((int)intensity.val[0] + (int)intensity.val[1] + (int)intensity.val[2]) > MAX_BACKGROUND_RGB) {
                colorRGB cellIntesity((int)intensity.val[2], (int)intensity.val[1], (int)intensity.val[0]); // BGR -> RGB
                
                if (colorsIDs[cellIntesity] > 0) {
                    grid[i][j] = colorsIDs[cellIntesity];
                    unorderedColorPairs[grid[i][j] - 1].second = point(i, j);
                    //colorPairs[grid[i][j] - 1].second = point(i, j);
                }
                else {
                    grid[i][j] = colorsIDs[cellIntesity] = ++currentColorID;
                    unorderedColorPairs.push_back({ point(i, j), point() });
                    //colorPairs.push_back({ point(i, j), point() });
                }
            }
        }
    }
    
    orderColorPairs(unorderedColorPairs);
}

void FlowSolver::orderColorPairs(vector<pair<point, point>>& unorderedColorPairs){
    //  Init. currentRow, currentCol
	int currentRow = 0;
	int currentCol = 0;

    //  Get nearest to currentRow, currentCol while checking both ends of the path and while ignoring the push pairs
    while (colorPairs.size() < unorderedColorPairs.size()) {
        int minDifference = 1e9;
        int minDifferenceIdx = -1;
        bool isFirstEnd = false;

        for (int i = 0; i < unorderedColorPairs.size(); ++i) {
			if (unorderedColorPairs[i].first.r == -1) {
				continue; // Already pushed
			}

            // Check first pair end
            int rowsDiff = abs(unorderedColorPairs[i].first.r - currentRow);
            int colsDiff = abs(unorderedColorPairs[i].first.c - currentCol);
            if (minDifference > rowsDiff+colsDiff) {
                minDifference = rowsDiff+colsDiff;
                minDifferenceIdx = i;
				isFirstEnd = true;
            }

            // Check second pair end
            rowsDiff = abs(unorderedColorPairs[i].second.r - currentRow);
            colsDiff = abs(unorderedColorPairs[i].second.c - currentCol);
            if (minDifference > rowsDiff+colsDiff) {
                minDifference = rowsDiff+colsDiff;
                minDifferenceIdx = i;
                isFirstEnd = false;
            }
        }

        // Push into color pairs
        colorPairs.push_back(unorderedColorPairs[minDifferenceIdx]);
        if (!isFirstEnd) {
            swap(colorPairs.back().first, colorPairs.back().second);
        }

        // Mark current pair (in the unorderedList) as pushed into the orderedPairsList
        unorderedColorPairs[minDifferenceIdx].first.r = -1;

        // Move currentRow, currentCol to path's other end
        currentRow = colorPairs.back().second.r;
        currentCol = colorPairs.back().second.c;
    }
}

void FlowSolver::printMaze() {
    for (int i = 0; i < gridRowsCount; ++i) {
        for (int j = 0; j < gridColsCount; ++j) {
            cout << setw(5) << grid[i][j];
        }
        cout << endl;
    }
    
    // Adding separation between before and after solving the maze
    cout << endl;
    for (int i = gridColsCount * 6; i >= 0; --i) cout << "-";
    cout << endl << endl;
}

string FlowSolver::getSolutionPaths() {
	string result = "";

	// Print statistics
    printMaze();
    cout << gridRowsCount << "x" << gridColsCount << endl;
    cout << "Block Width: " << singleBlockWidth << ", Block Height: " << singleBlockHeight << endl;
    cout << "Recursive calls count: " << recursiveCalls << endl;

    // Assume: pen starts at (0, 0)
    int currentRow = 0, currentCol = 0;
    
    for (int i = 0; i < colorPathes.size(); i++) {
		// Generate the path from the ending cell of the previous color pair to the starting
		// cell of the current color pair
		int targetRow = colorPairs[i].first.r;
		int targetCol = colorPairs[i].first.c;
		int diffStepsRows = targetRow - currentRow;
		int diffStepsCols = targetCol - currentCol;

        for (int k = 0; k < abs(diffStepsRows); ++k) {
			result += (diffStepsRows < 0 ? "^" : "v");
        }
        for (int k = 0; k < abs(diffStepsCols); ++k) {
			result += (diffStepsCols < 0 ? "<" : ">");
        }
        
		// Press instruction
		result += "P";
        
        for (int j = 0; j < colorPathes[i].size(); ++j) {
            switch (colorPathes[i][j])
            {
                case DOWN:
					result += "v";
                    break;
                case UP:
					result += "^";
                    break;
                case RIGTH:
					result += ">";
                    break;
                case LEFT:
					result += "<";
                    break;
            }
        }

		// Release instruction
		result += "R";
        
        // Set current pen point on the grid (preparing for the next color pair)
        currentRow = colorPairs[i].second.r;
        currentCol = colorPairs[i].second.c;
    }

    return result;
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

bool FlowSolver::_solve(int row, int col, int prvRow, int prvCol, int pairIdx) {
    ++recursiveCalls;
    
    // Connected two pins of the same colors together
    if (row == colorPairs[pairIdx].second.r && col == colorPairs[pairIdx].second.c) {
        if (++pairIdx < colorPairs.size()) {
            // Try to connect the next color pair
            int toX = colorPairs[pairIdx].first.r;
            int toY = colorPairs[pairIdx].first.c;
            return _solve(toX, toY, -1, -1, pairIdx);
        }
        else {
            // Finished connecting every pair of colors
            return true;
        }
    }
    
    // Cell is already occupied so just return false
    if (row != colorPairs[pairIdx].first.r || col != colorPairs[pairIdx].first.c) {
        if (grid[row][col] != 0) {
            return false;
        }
    }
    
    // Try to fill the current cell with the current color
    int temp = grid[row][col];
    grid[row][col] = grid[colorPairs[pairIdx].first.r][colorPairs[pairIdx].first.c];
    
    // If the current state is unsolvable then undo the filling and no need for further branching
    if (!solvable(row, col, pairIdx)) {
        grid[row][col] = temp;
        return false;
    }
    
    // Try to flow in the 4 directions and check if the maze is solvable
    for (int i = 0; i < 4; ++i) {
        int toR = row + dx[i];
        int toC = col + dy[i];
        
        if ((toR != prvRow || toC != prvCol) && valid(toR, toC) && _solve(toR, toC, row, col, pairIdx)) {
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