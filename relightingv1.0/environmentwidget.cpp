#include "environmentwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "data.h"
#include "vectorimage.hpp"
#include "InputDialog.h"

#include <string>

#include <QPoint>

string num2str(double i)

{

	stringstream ss;

	ss << i;

	return ss.str();

}

environmentWidget::environmentWidget(QWidget *parent /* = 0 */)
{
	glwidget = new GLWidget();

	QHBoxLayout *mainlayout = new QHBoxLayout();
	mainlayout->addWidget(glwidget); 
	setLayout(mainlayout);
}

environmentWidget::~environmentWidget()
{

}

void environmentWidget::init(QString imgname, string maskname, string plyname)
{
	est = new Estimation(plyname.c_str(), imgname.toStdString(), maskname);
	Eigen::Vector3d lightdir = est->estimateLight();
	est->calParams(lightdir, ts, th);

//	test();

	//add final result 
	if (true) {
		VectorImage resultImg;
		resultImg = resultImg.fromFile(imgname.toStdString());

		VectorImage imgdata, maskdata;
		imgdata = imgdata.fromFile(imgname.toStdString());
		maskdata = maskdata.fromFile(maskname);
		QImage imgSp, imgIkHp;
		imgSp.load(imgname + ".Sp.jpg");
		imgIkHp.load(imgname + ".Sp.jpg");

		//select the target model, image and mask
		InputDialog *dlg = new InputDialog(this);
		dlg->exec();
		
		Estimation *targetEst = new Estimation(tplyname.toStdString(), timgname.toStdString(), tmaskname.toStdString());
		Eigen::Vector3d targetLight = targetEst->estimateLight();

		//rendering
		//put the origin model to target image
		QRgb rgb;
		int i = 0;
		for (size_t x = 0; x < resultImg.width(); x++) {
			for (size_t y = 0; y < resultImg.height(); y++) {
				if (maskdata.pixel(x, y) == BLACK) 	continue;
				//I = Ia ka + Ip Sp (kd <l, n> + ks Hp)
				if (i >= est->normals->size()) break;
				rgb = imgSp.pixel(QPoint(x, y)) *(est->calAngle(targetLight, Eigen::Vector3d(est->normals->at(i).normal_x, est->normals->at(i).normal_y, est->normals->at(i).normal_z)) + imgIkHp.pixel(QPoint(x, y)));
				i++;
				resultImg.setPixel(x, y, rgb);
			}
		}

		//save final result
		QImage resultQImg = resultImg.toQImage(resultImg);
		resultQImg.save("result.jpg");
		
		cout << "Finish all." << endl;
	}
}

void environmentWidget::test()
{
	vector<string> testdatastring;
	testdatastring.push_back("buddha");
	testdatastring.push_back("cat");
	testdatastring.push_back("gray");
	testdatastring.push_back("horse");
	testdatastring.push_back("owl");
	testdatastring.push_back("rock");

	ofstream of("message.txt");
	streambuf *streams = cout.rdbuf();
	cout.rdbuf(of.rdbuf());
	
	string plystring, maskstring, imgstring;
	for (int i = 0; i < testdatastring.size(); i++) {
		plystring = "./testdataset/" + testdatastring.at(i) + ".ply";
		maskstring = "./testdataset/" + testdatastring.at(i) + "/"+ testdatastring.at(i) + ".mask.png";
		for (int j = 0; j < 12; j++) {
			imgstring = "./testdataset/" + testdatastring.at(i) + "/" + testdatastring.at(i) + "." + num2str(j) + ".png";


			est = new Estimation(plystring, imgstring, maskstring);
			Eigen::Vector3d lightdir = est->estimateLight();
			est->calParams(lightdir, ts, th);
		}
	}

	cout.rdbuf(of.rdbuf());
}