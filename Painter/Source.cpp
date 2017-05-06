#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include "Painter.h"
using namespace std;

#define IMAGE_FILE          "Images/image.jpg"
#define BI_IMAGE_FILE       "Images/bi_image.jpg"
#define INSTRUCTIONS_FILE   "instructions.txt"

/**
 * Used to boost reading/writing from/to the console
 */
inline void boostIO() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
}

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

	// Get image path
	if (argc >= 2) {
		imagePath = argv[1];
	}
	// Get instructions path
	if (argc >= 3) {
		imagePath = argv[2];
	}

	try {
		Painter painter(IMAGE_FILE);
		imwrite(BI_IMAGE_FILE, painter.image);
		string instructions = painter.drawingInstructions();
		//cout << "Instructions: " << endl << instructions << endl;
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

inline void outputInstructions(const string& instructions, const string& path) {
	// Output instructions into a file to be send to Arduino
	ofstream fout(path);
	fout << instructions;
	fout.close();
}