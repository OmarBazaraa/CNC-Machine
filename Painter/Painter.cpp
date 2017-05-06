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
	int currentRow = 0;
	int currentCol = 0;
	bool pressed = false;
	string instructions;

	// Generate drawing instructions
	for (int i = 0; i < image.rows; ++i) {
		int leftCol = -1;
		int rightCol = -1;

		// Detect first black pixel from the left
		for (int j = 0; j < image.cols; ++j) {
			// If white pixel then continue
			if ((bool)image.at<uchar>(i, j)) {
				continue;
			}

			leftCol = j;
			break;
		}

		// Detect first black pixel from the right
		for (int j = image.cols - 1; j >= 0; --j) {
			// If white pixel then continue
			if ((bool)image.at<uchar>(i, j)) {
				continue;
			}

			rightCol = j;
			break;
		}

		// Continue if no black pixels in this row
		if (leftCol == -1 || rightCol == -1) {
			continue;
		}

		int startCol = leftCol;
		int step = 1;

		// Detect whether to start from right or left is shorter
		if (abs(rightCol - currentCol) < abs(leftCol - currentCol)) {
			startCol = rightCol;
			step = -1;
		}

		for (int j = startCol; j >= leftCol && j <= rightCol; j += step) {
			// If white pixel then continue
			if ((bool)image.at<uchar>(i, j)) {
				continue;
			}

			int diffRow = i - currentRow;
			int diffCol = j - currentCol;

			if (pressed && abs(diffCol) + abs(diffRow) > 1) {
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

			currentRow = i;
			currentCol = j;
		}
	}

	instructions += "R";

	return instructions;
}

string Painter::compressInstructions(const string& instructions) {
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

bool Painter::checkValidInstructions(const string& instructions) {
	cv::Mat retrievedImage(image.rows, image.cols, CV_8U, cv::Scalar(255));

	int i = 0, j = 0;
	bool pressed = false;

	for (char c : instructions) {
		if (c == 'P')
			pressed = true;
		else if (c == 'R')
			pressed = false;
		else if (c == '>')
			++j;
		else if (c == '<')
			--j;
		else if (c == '^')
			--i;
		else if (c == 'v')
			++i;

		cout << i << ' ' << j << endl;
		
		if (pressed) {
			retrievedImage.at<uchar>(i, j) = (uchar)0;
		}
	}

	return compareImages(retrievedImage, image);
}

bool Painter::compareImages(const cv::Mat& img1, const cv::Mat& img2) {
	// treat two empty mat as identical
	if (img1.empty() && img2.empty())
		return true;

	// if dimensions of two mat is not identical, these two mat is not identical
	if (img1.cols != img2.cols || img1.rows != img2.rows || img1.dims != img2.dims)
		return false;

	// Compare every pixel
	for (int i = 0; i < img1.rows; ++i)
		for (int j = 0; j < img1.cols; ++j)
			if (img1.at<bool>(i, j) != img2.at<bool>(i, j))
				return false;

	return true;
}