#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H
#define WIN64

#include <QThread>
#include <QImage>
#include <QPixmap>
#ifdef WIN64
#include <xiApi.h>
#else
#include <m3api/xiApi.h>
#endif

#include <opencv2/opencv.hpp>

using namespace cv;
class captureThread : public QThread
{
    Q_OBJECT

public:
   //explicit captureThread(QObject *parent = 0);
   captureThread();

signals:
    void getFPS(unsigned fps);
    void getImage(void *buffer);
    void getImage(QImage image);
    void getImageRaw(QImage image);
    void getImageSize(unsigned width, unsigned height);

public slots:
    void saveFrames();

private:
    void run();
    XI_IMG image;
    Mat *frame, *frame_color;
    HANDLE handle = NULL;
    XI_RETURN stat = XI_OK;
    unsigned long buffer_size;
    unsigned int frames_in_sec, time_start, time_stop, time_lapsed;
    QImage *captured_frames, temp;
    QString mystring;
    float fps;
    bool save_frames= FALSE;
    int framenum;
};



#endif // CAPTURETHREAD_H

