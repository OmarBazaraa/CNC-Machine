#include <iostream>
#include <ctime>
#include "FlowSolver.h"
using namespace std;

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
int main() {
    boostIO();
    
    try {
        FlowSolver solver("/Users/ibrahimradwan/Desktop/mazes/maze8x8.png");
        solver.printMaze();
        solver.solve();
        solver.printSolution();
    }
    catch (const exception& ex) {
        cout << "ERROR::" << ex.what() << endl;
    }
    return 0;
}
