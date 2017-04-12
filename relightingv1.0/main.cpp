#include "mainwin.h"
#include <QtWidgets/QApplication>

#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[])
{
	//output all the message to txt file
	ofstream of("message.txt");
//	streambuf *streams = cout.rdbuf();
//	cout.rdbuf(of.rdbuf());

	QApplication a(argc, argv);
	MainWin w;
	w.show();
	return a.exec();

//	cout.rdbuf(streams);
}
