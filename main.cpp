#include <QApplication>
#include <QLabel>
#include <xiApi.h>

#include "captureplot.h"
#include "capturethread.h"
#include "renderimage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    captureThread *mythread = new captureThread();
    renderImage *camimage= new renderImage();
    camimage->show();

    QObject::connect(mythread,SIGNAL(getImage(void*)),camimage,SLOT(receiveBitmap(void*)));

    mythread->start(QThread::NormalPriority);
    return a.exec();
}
