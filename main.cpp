#include <QApplication>
#include <QLabel>
#include <xiApi.h>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>

#include "captureplot.h"
#include "capturethread.h"
#include "renderimage.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    captureThread *mythread = new captureThread();
    renderImage *camimage= new renderImage();
    //camimage->show();

    QPushButton *saveButton= new QPushButton("save frames");

    //QObject::connect(mythread, SIGNAL(getImage(void*)), camimage, SLOT(receiveBitmap(void*)));
    QObject::connect(mythread, SIGNAL(getImage(QImage)), camimage, SLOT(receiveBitmap(QImage)));
    //QObject::connect(mythread, SIGNAL(getImageRaw(QImage)), camimage, SLOT(receiveBitmapRaw(QImage)));

    QObject::connect(saveButton, SIGNAL(pressed()), mythread, SLOT(saveFrames()));
    //saveButton->show();

    QGridLayout *simplelayout= new QGridLayout();
    simplelayout->addWidget(camimage,0,0,1,1,Qt::AlignHCenter);
    simplelayout->addWidget(saveButton,1,0,1,1, Qt::AlignHCenter);
    QWidget *display= new QWidget();
    display->setLayout(simplelayout);
    display->show();

    mythread->start(QThread::HighestPriority);
    return a.exec();
}
