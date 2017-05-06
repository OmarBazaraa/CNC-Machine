// STL libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <exception>

// OpenCV libraries
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

#define BLACK_WHITE_THRESHOLD	180

class Painter
{
public:
	Mat image;

public:
	/**
	 * Constructor
	 */
	Painter(const string& path);

	/**
	 * Destructor
	 */
	~Painter();

private:
	/**
	 * Loads image from the given path into a binary matrix representing black and white colors
	 */
	bool loadBinaryImage(const string& path);

public:
	/** 
	 * Returns the drawing instructions of the given image to be drawn be sent to the Arduino
	 */
	string drawingInstructions();

private:
	string compressInstructions(const string& instructions);

	bool checkValidInstructions(const string& instructions);

	bool compareImages(const cv::Mat& img1, const cv::Mat& img2);
};