#include <iostream>
using namespace std;

#include "estimate.h"

#include <string>

string num2str(double i)

{

	stringstream ss;

	ss << i;

	return ss.str();

}

int main() {

	vector<string> testdatastring;
	testdatastring.push_back("buddha");
	testdatastring.push_back("cat");
	testdatastring.push_back("gray");
	testdatastring.push_back("horse");
	testdatastring.push_back("owl");
	testdatastring.push_back("rock");

	string plystring, imgstring, maskstring;

	ofstream of("testlight.txt");
//	streambuf *streams = cout.rdbuf();
//	cout.rdbuf(of.rdbuf());

	for (int i = 0; i < testdatastring.size(); i++) {
		plystring = "./testdataset/" + testdatastring.at(i) + ".ply";

		maskstring = "./testdataset/" + testdatastring.at(i) + "/" + testdatastring.at(i) + ".mask.png";
		for (int j = 0; j < 12; j++) {
			imgstring = "./testdataset/" + testdatastring.at(i) + "/" + testdatastring.at(i) + "." + num2str(j) + ".png";

			of << i << endl << j << endl;
			Estimation est(plystring, imgstring, maskstring);
			of << est.estimateLight() << endl;
		}
	}

//	cout.rdbuf(of.rdbuf());

	return 0;
}

int testnormal() {
	vector<string> testdatastring;
	testdatastring.push_back("buddha");
	testdatastring.push_back("cat");
	testdatastring.push_back("gray");
	testdatastring.push_back("horse");
	testdatastring.push_back("owl");
	testdatastring.push_back("rock");

	char imgstring[80], maskstring[80];
	string plystring;
	for (int i = 0; i < testdatastring.size(); i++) {
		plystring = "./testdataset/" + testdatastring.at(i) + ".ply";

		cout << plystring << endl;
		
		Estimation est(plystring, imgstring, maskstring);
		est.calNormal(plystring);
		break;
	}
}