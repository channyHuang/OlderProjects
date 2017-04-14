#include "readAndRun.hpp"

int main() {

	mainTrack();

	double lambda = fusion();
	cout << "*** Lambda : *** " << lambda << endl;

	mainReconstruct2();

	return 0;
}