#include <QApplication>
#include <QLabel>
#include <xiApi.h>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QSpinBox>
#include "captureplot.h"
#include "capturethread.h"
#include "renderimage.h"

#define EXPOSURE_DEFAULT 2000
#define EXPOSURE_MIN 100
#define EXPOSURE_MAX 200000

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    captureThread *mythread = new captureThread();
    renderImage *camimage= new renderImage();
    //camimage->show();

    QLabel *exposureLabel = new QLabel("Exposure");
    QSpinBox *exposureBox = new QSpinBox();
    exposureBox->setMinimum(EXPOSURE_MIN);
    exposureBox->setMaximum(EXPOSURE_MAX);

    QLabel *rateLabel = new QLabel("Frame rate");
    QSpinBox *rateBox = new QSpinBox();
    rateBox->setMaximum(600);
    rateBox->setReadOnly(true);

    QPushButton *saveButton= new QPushButton("save frames");

    //QObject::connect(mythread, SIGNAL(getImage(void*)), camimage, SLOT(receiveBitmap(void*)));
    QObject::connect(mythread, SIGNAL(getImage(QImage)), camimage, SLOT(receiveBitmap(QImage)));
    QObject::connect(exposureBox, SIGNAL(valueChanged(int)), mythread, SLOT(setExposure(int)));
    QObject::connect(saveButton, SIGNAL(pressed()), mythread, SLOT(saveFrames()));
    QObject::connect(mythread, SIGNAL(getFPS(uint)), rateBox, SLOT(setValue(int)));
    //saveButton->show();

    QGridLayout *simplelayout= new QGridLayout();
    simplelayout->addWidget(camimage, 0, 0, 1, 5, Qt::AlignHCenter);
    simplelayout->addWidget(exposureLabel, 1, 0, 1, 1, Qt::AlignRight);
    simplelayout->addWidget(exposureBox, 1, 1, 1, 1, Qt::AlignLeft);
    simplelayout->addWidget(rateLabel, 1, 2, 1, 1, Qt::AlignRight);
    simplelayout->addWidget(rateBox, 1, 3, 1, 1, Qt::AlignLeft);
    simplelayout->addWidget(saveButton, 1 ,4, 1, 1, Qt::AlignHCenter);
    QWidget *display= new QWidget();
    display->setLayout(simplelayout);
    display->show();

    exposureBox->setValue(EXPOSURE_DEFAULT);
//    mythread->setExposure(EXPOSURE_DEFAULT);
//  mythread->start(QThread::HighestPriority);
    return a.exec();
}
