#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QImage>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#ifdef __linux__
#include <m3api/xiApi.h>
#include <sys/time.h>
#elif _WIN32
#include <xiApi.h>
#endif
#include <opencv2/opencv.hpp>
#include <marker.h>

#define MARKERS_COUNT 20
using namespace cv;

class captureThread : public QThread
{
    Q_OBJECT

public:
   explicit captureThread(QObject *parent = 0);
   captureThread();
   captureThread(int dev);

signals:
    void getFPS(int fps);
    void getImage(void *buffer);
    void getImage(QImage image);
    void getImageSize(unsigned width, unsigned height);
    void getSeparator(int x0, int y0, int x1, int y1);

public slots:
    void saveFrames();
    void setExposure(int val);
    void setFramesToSave(int val);
    void setThreshold(int val);
    void setGain(double val);
    int calculateCentroids();

private:

    char devicenum;
    void run();
    XI_IMG image;
    Mat *frame, *frame_color, *frame_buffer;
    Mat *captured_frames;
    HANDLE handle = NULL;
    XI_RETURN stat = XI_OK;
    unsigned long buffer_size;
    unsigned int frames_in_sec, time_start, time_stop, time_lapsed;
    QImage temp;
    QString mystring;
    float fps;
    bool save_frames= FALSE;
    int framenum, framenum_max;
    float moment_x[MARKERS_COUNT], moment_y[MARKERS_COUNT], mass[MARKERS_COUNT];
    float moment_x_temp, moment_y_temp, mass_temp;
    struct timeval *timestamp;
    SimpleBlobDetector detector;
    // time logging purpose
    QFile *logfile;
    QTextStream *streamout;
    int clock_freq;

    std::vector<KeyPoint> keypoints;
    Mat invert;
    marker markers[MARKERS_COUNT];
    int offset, temp_x, temp_y;
};



#endif // CAPTURETHREAD_H

