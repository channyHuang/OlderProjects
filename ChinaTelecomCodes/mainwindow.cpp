#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include "data.h"
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QClipboard>

#include <QSize>
#include <QRect>
#include <QPainter>


windows::windows(QWidget *parent/* =0 */,int i)
	: ui(new Ui::windows),QMainWindow(parent)
{
	this->resize(totalwidth,totalheight);
	this->setMinimumSize(totalwidth,totalheight);
	this->setMaximumSize(totalwidth,totalheight);
	saveaction=new QAction(QStringLiteral("保存"),this);
	saveaction->setShortcut(QKeySequence::Save);
	saveaction->setStatusTip(QStringLiteral("保存图表"));
	saveaction->setIcon(QIcon("./image/save.png"));
	connect(saveaction,SIGNAL(triggered()),this,SLOT(saveimg()));

	printaction=new QAction(QStringLiteral("打印"),this);
	printaction->setShortcut(QKeySequence::Print);
	printaction->setStatusTip(QStringLiteral("打印本图表"));
	printaction->setIcon(QIcon("./image/print.png"));
	connect(printaction,SIGNAL(triggered()),this,SLOT(printimg()));

	copyaction=new QAction(QStringLiteral("复制"),this);
	copyaction->setShortcut(QKeySequence::Copy);
	copyaction->setStatusTip(QStringLiteral("复制本图表到剪切板"));
	copyaction->setIcon(QIcon("./image/copy.png"));
	connect(copyaction,SIGNAL(triggered()),this,SLOT(copyimg()));

	closeaction=new QAction(QStringLiteral("退出"),this);
	closeaction->setShortcut(QKeySequence::Close);
	closeaction->setStatusTip(QStringLiteral("退出本窗口"));
	closeaction->setIcon(QIcon("./image/close.png"));
	connect(closeaction,SIGNAL(triggered()),this,SLOT(close()));

	aboutaction=new QAction(QStringLiteral("关于"),this);
	aboutaction->setStatusTip(QStringLiteral("关于本软件"));
	aboutaction->setIcon(QIcon("./image/about.png"));
	connect(aboutaction,SIGNAL(triggered()),this,SLOT(about()));

	QMenu *editmenu=menuBar()->addMenu(tr("&Edit"));
	editmenu->addAction(saveaction);
	editmenu->addAction(printaction);
	editmenu->addAction(copyaction);
	QMenu *windowsmenu=menuBar()->addMenu(tr("Window"));
	windowsmenu->addAction(closeaction);
	windowsmenu->addAction(aboutaction);
	QToolBar *tool=addToolBar(tr("&Toolbar"));
	tool->addAction(saveaction);
	tool->addAction(printaction);
	tool->addAction(copyaction);
	tool->addAction(closeaction);
	tool->addAction(aboutaction);

	statusBar();

	num=i;
	if (num < 10) {
		singlwidget = new SingleWidget(this, num);
		setCentralWidget(singlwidget);
	}
	else if (num == 10) {
		timecity = new timeCity(this);
		setCentralWidget(timecity);
	}
	else if (num == 11) {
		glwidget = new drawidget(this, num);
		setCentralWidget(glwidget);
	}
	else {

	}
}

windows::~windows()
{
	delete ui;
}

bool windows::saveimg()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("save"), tr("untitled.png"), tr("*.png;; *.jpg;; *.bmp"));
	if(filename.isEmpty()) return false;
		QImage image(singlwidget->size(),QImage::Format_ARGB32);
		QPainter painter(&image);
		singlwidget->render(&painter);
		painter.end();
		image.save(filename);
		statusBar()->showMessage(tr("saving successed"),2000);
		return true;
}

void windows::printimg()
{
	QPrinter printer;
	QImage image(singlwidget->size(),QImage::Format_ARGB32);
	QPainter painter(&image);
	singlwidget->render(&painter);
	painter.end();
	QPrintDialog printdialog(&printer,this);
	if(printdialog.exec()) {
		QPainter painter;
		painter.begin(&printer);
		QRect rect=painter.viewport();
		QSize size=image.size();
		size.scale(rect.size(),Qt::KeepAspectRatio);
		painter.setViewport(rect.x(),rect.y(),size.width(),size.height());
		painter.setWindow(image.rect());
		painter.drawImage(0,0,image);
	}
}
void windows::copyimg()
{
	QImage image(singlwidget->size(),QImage::Format_ARGB32);
	QPainter painter(&image);
	singlwidget->render(&painter);
	painter.end();
	QClipboard *clipboard=QApplication::clipboard();
	clipboard->setImage(image);
}
void windows::about()
{
	QMessageBox::about(NULL,QStringLiteral("关于本软件："),QStringLiteral("本软件主要用于读取中国电信的日志数据文件，并对其中的变量进行单维度和多维度分析，作出合适的统计图表。"));
}