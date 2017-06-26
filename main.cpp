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
#define EXPOSURE_MIN 28
#define EXPOSURE_MAX 200000
#define FRAMENUM_MAX 3000

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
    rateBox->setMaximum(2000);
    rateBox->setReadOnly(true);

    QLabel *framestoSave = new QLabel("Saved Frames");
    QSpinBox *framesBox = new QSpinBox();
    framesBox->setMaximum(FRAMENUM_MAX);
    framesBox->setMinimum(2);

    QPushButton *saveButton= new QPushButton("save frames");

    //QObject::connect(mythread, SIGNAL(getImage(void*)), camimage, SLOT(receiveBitmap(void*)));
    QObject::connect(mythread, SIGNAL(getImage(QImage)), camimage, SLOT(receiveBitmap(QImage)));

    QObject::connect(exposureBox, SIGNAL(valueChanged(int)), mythread, SLOT(setExposure(int)));
    QObject::connect(saveButton, SIGNAL(pressed()), mythread, SLOT(saveFrames()));
    QObject::connect(mythread, SIGNAL(getFPS(int)), rateBox, SLOT(setValue(int)));
    QObject::connect(framesBox, SIGNAL(valueChanged(int)), mythread, SLOT(setFramesToSave(int)));

    QGridLayout *simplelayout= new QGridLayout();
    simplelayout->addWidget(camimage, 0, 0, 1, 7, Qt::AlignHCenter);
    simplelayout->addWidget(exposureLabel, 1, 0, 1, 1, Qt::AlignRight);
    simplelayout->addWidget(exposureBox, 1, 1, 1, 1, Qt::AlignLeft);
    simplelayout->addWidget(rateLabel, 1, 2, 1, 1, Qt::AlignRight);
    simplelayout->addWidget(rateBox, 1, 3, 1, 1, Qt::AlignLeft);
    simplelayout->addWidget(framestoSave, 1, 4, 1,1, Qt::AlignHCenter);
    simplelayout->addWidget(framesBox, 1, 5, 1, 1, Qt::AlignHCenter);
    simplelayout->addWidget(saveButton, 1 ,6, 1, 1, Qt::AlignHCenter);

    QWidget *display= new QWidget();
    display->setLayout(simplelayout);
    display->show();

//    exposureBox->setValue(EXPOSURE_DEFAULT);
    framesBox->setValue(1000);
    mythread->start(QThread::HighestPriority);
    return a.exec();
}
