#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QImage>
#include <QPixmap>
#ifdef __linux__
#include <m3api/xiApi.h>
#elif _WIN32
#include <xiApi.h>
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
    void getImageSize(unsigned width, unsigned height);

public slots:
    void saveFrames();
    void setExposure(int val);

private:
    void run();
    XI_IMG image;
    Mat *frame, *frame_color;
    Mat *captured_frames;
    HANDLE handle = NULL;
    XI_RETURN stat = XI_OK;
    unsigned long buffer_size;
    unsigned int frames_in_sec, time_start, time_stop, time_lapsed;
    QImage temp;
    QString mystring;
    float fps;
    bool save_frames= FALSE;
    int framenum;


};



#endif // CAPTURETHREAD_H

