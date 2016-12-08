#include "capturethread.h"
#include <m3api/xiApi.h>
#include <string.h> //not sure
#include <QDateTime>
#include <QApplication>

#define RENDER_INTERVAL 10
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define FRAMENUM_MAX 1000
#define OPT_SAVE

/*
captureThread::captureThread(QObject *parent) : QThread(parent){
}*/

captureThread::captureThread()
{
    //captured_frames = new QImage()[FRAMENUM_MAX];
    if(xiOpenDevice(0, &handle) != XI_OK) exit(1);
    xiSetParamInt(handle, XI_PRM_EXPOSURE, 1000); // us
    xiSetParamFloat(handle, XI_PRM_GAIN, 7.4); // -3.5 to 7.4
    xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24); // simply cause I can
    xiSetParamInt(handle, XI_PRM_OFFSET_X, 0);
    xiSetParamInt(handle, XI_PRM_OFFSET_Y, 0);
    xiSetParamInt(handle, XI_PRM_WIDTH, IMAGE_WIDTH);
    xiSetParamInt(handle, XI_PRM_HEIGHT, IMAGE_HEIGHT);
    xiSetParamInt(handle, XI_PRM_AUTO_WB, 1);
 //   xiSetParamInt(handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);// set acquisition to frame rate mode
 //   xiSetParamInt(handle, XI_PRM_FRAMERATE, 90);// Requested fps

    image.size = sizeof(XI_IMG);
    image.bp = NULL;
    image.bp_size = 0;
    buffer_size = IMAGE_WIDTH*IMAGE_HEIGHT*3;
    emit getImageSize(image.width, image.height);
    qDebug("size %d %d",image.width, image.height);
    captured_frames = new QImage[FRAMENUM_MAX];
}

void captureThread::run(){
   xiSetParamInt(handle, XI_PRM_WIDTH, IMAGE_WIDTH);
   xiSetParamInt(handle, XI_PRM_HEIGHT, IMAGE_HEIGHT);
   xiStartAcquisition(handle);
   time_start= QDateTime::currentDateTime().toMSecsSinceEpoch();
   for (int framenum=0; framenum<FRAMENUM_MAX; framenum++){
//   while (1){
       xiGetImage(handle, 1, &image); // Capture Image //timeout
       temp= QImage(static_cast<unsigned char*>(image.bp), IMAGE_WIDTH, IMAGE_HEIGHT, 3*IMAGE_WIDTH, QImage::Format_RGB888);
       captured_frames[framenum]= temp.copy(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
       time_stop = QDateTime::currentDateTime().toMSecsSinceEpoch();
       time_lapsed= time_stop- time_start;
       if (time_lapsed > 1000){ // 1 sec lapsed
            emit getFPS(frames_in_sec);
            qDebug("fps-counter %d",frames_in_sec);
            time_start= time_stop;
            frames_in_sec= 0;
            xiGetParamFloat(handle, XI_PRM_FRAMERATE, &fps);//Acquired fps
            qDebug("fps-board %f",fps);

       }
       else frames_in_sec++;
       if (!(frames_in_sec%RENDER_INTERVAL)) emit getImage(image.bp);
#ifdef OPT_SAVE
   }
   for (int framenum=0; framenum<FRAMENUM_MAX; framenum++){
        qDebug("saving %d",framenum);
        captured_frames[framenum].rgbSwapped().save(mystring.sprintf("xi%04d.png",framenum),"PNG",0);
#endif
   }
   exit(0);
   QApplication::exit(0);
}
