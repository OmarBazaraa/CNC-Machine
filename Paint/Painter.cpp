#include "Painter.h"

Painter::Painter(const string& path) {
	// Load the image into matrix image
	if (!loadBinaryImage(path)) {
		string errorMessage = "Could not load the image at: " + path;
		cout << errorMessage << endl;

		throw exception(errorMessage.c_str());
	}
}

Painter::~Painter() {
	image.deallocate();
}

bool Painter::loadBinaryImage(const string& path) {
	// Load colored image from file
	cv::Mat rgbMat = imread(path, CV_LOAD_IMAGE_COLOR);

	// Check for invalid input
	if (rgbMat.empty() || !rgbMat.data) {
		return false;
	}

	// Grayscale matrix
	cv::Mat grayMat(rgbMat.size(), CV_8U);

	// Convert BGR to Gray
	cv::cvtColor(rgbMat, grayMat, CV_BGR2GRAY);

	// Resizing image
	cv::Mat resizedMat;
	cv:resize(grayMat, resizedMat, cv::Size(0, 0), 0.3, 0.3);
	
	// Apply thresholding
	cv::adaptiveThreshold(resizedMat, image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 9, 4);
	return true;
}

string Painter::drawingInstructions() {
	vector<pair<int, int>> blackPixelsPos;

	// Get black pixels positions
	for (int i = 0; i < image.rows; ++i) {
		for (int j = 0; j < image.cols; ++j) {
			// If white pixel then continue
			if ((bool)image.at<uchar>(i, j)) {
				continue;
			}

			blackPixelsPos.push_back({ i, j });
		}
	}

	int currentRow = 0;
	int currentCol = 0;
	bool pressed = false;
	string instructions;

	// Generate drawing instructions
	for (int i = 0; i < blackPixelsPos.size(); ++i) {
		int row = blackPixelsPos[i].first;
		int col = blackPixelsPos[i].second;

		int diffRow = row - currentRow;
		int diffCol = col - currentCol;
		
		if (abs(diffCol) + abs(diffRow) > 1 && pressed) {
			instructions += "R";
			pressed = false;
		}

		if (abs(diffCol) > 0) {
			instructions.append(abs(diffCol), diffCol > 0 ? '>' : '<');
		}

		if (abs(diffRow) > 0) {
			instructions.append(abs(diffRow), diffRow > 0 ? 'v' : '^');
		}

		if (!pressed) {
			instructions += "P";
			pressed = true;
		}

		currentRow = row;
		currentCol = col;
	}

	instructions += "R";


	string result;
	int cnt = 1;
	char prv = instructions[0];

	// Compress instructions
	for (int i = 1; i < instructions.size(); ++i) {
		if (prv == instructions[i]) {
			++cnt;
		}
		else {
			result += prv;

			if (prv != 'P' && prv != 'R') {
				result += " " + to_string(cnt);
			}

			result += "\n";
			
			prv = instructions[i];
			cnt = 1;
		}
	}
	
	return result;
}