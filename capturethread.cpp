#include "capturethread.h"
#include <m3api/xiApi.h>
#include <string.h> //not sure
#include <QDateTime>

#define RENDER_INTERVAL 4
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define FRAMENUM_MAX 500
// ximea parameter list goes here
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
       xiGetImage(handle, 40, &image); // Capture Image //timeout
// saving to buffer
    captured_frames[framenum]= QImage(static_cast<unsigned char*>(image.bp), IMAGE_WIDTH, IMAGE_HEIGHT, 3*IMAGE_WIDTH, QImage::Format_RGB888);
    time_stop = QDateTime::currentDateTime().toMSecsSinceEpoch();
       time_lapsed= time_stop- time_start;
       if (time_lapsed > 1000){ // 1 sec lapsed
            emit getFPS(frames_in_sec);
            qDebug("frame %d",frames_in_sec);
            time_start= time_stop;
            frames_in_sec= 0;
       }
       else frames_in_sec++;
       if (!(frames_in_sec%RENDER_INTERVAL)) emit getImage(image.bp);
    }
   for (int framenum=0; framenum<FRAMENUM_MAX; framenum++){
        temp = captured_frames[framenum].rgbSwapped();
        temp.save(mystring.sprintf("xi%04d.png",framenum),"PNG",0);

   }

}
