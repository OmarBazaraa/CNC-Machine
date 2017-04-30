#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include "Painter.h"
using namespace std;

#define IMAGE_FILE          "image.png"
#define BI_IMAGE_FILE       "bi_image.jpg"
#define INSTRUCTIONS_FILE   "instructions.txt"

/**
 * Used to boost reading/writing from/to the console
 */
inline void boostIO() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
}

/**
 * Main function
 */
int main(int argc, char* argv[]) {
	boostIO();

	int startTime = clock();

	try {
		Painter painter(IMAGE_FILE);
		imwrite(BI_IMAGE_FILE, painter.image);
		string instructions = painter.drawingInstructions();
		//cout << "Instructions: " << endl << instructions << endl;

		// Output instructions into a file to be send to Arduino
		ofstream fout(INSTRUCTIONS_FILE);
		fout << instructions;
		fout.close();
	}
	catch (const exception& ex) {
		cout << "ERROR::" << ex.what() << endl;
	}

	int stopTime = clock();
	cout << endl << "Time: " << (stopTime - startTime) / double(CLOCKS_PER_SEC) * 1000.0 << "ms" << endl;

	return 0;
}