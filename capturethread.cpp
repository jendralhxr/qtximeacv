#include "capturethread.h"
#include <string.h>
#include <QDateTime>

#define RENDER_INTERVAL 10
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define EXPOSURE 3000 // us
#define FRAMENUM_MAX 2000

using namespace cv;

char filename[20];
/*
captureThread::captureThread(QObject *parent) : QThread(parent){
}*/

captureThread::captureThread()
{
    framenum_max= FRAMENUM_MAX;
    if(xiOpenDevice(0, &handle) != XI_OK) exit(1);
    xiSetParamInt(handle, XI_PRM_EXPOSURE, EXPOSURE); // us
    xiSetParamFloat(handle, XI_PRM_GAIN, 7.4); // -3.5 to 7.4
    //xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24); // simply cause I can
    xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RAW8); // faster
    xiSetParamInt(handle, XI_PRM_OFFSET_X, 0);
    xiSetParamInt(handle, XI_PRM_OFFSET_Y, 0);
    xiSetParamInt(handle, XI_PRM_WIDTH, IMAGE_WIDTH);
    xiSetParamInt(handle, XI_PRM_HEIGHT, IMAGE_HEIGHT);
    xiSetParamInt(handle, XI_PRM_AUTO_WB, 0);
 //   xiSetParamInt(handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);// set acquisition to frame rate mode
 //   xiSetParamInt(handle, XI_PRM_FRAMERATE, 90);// Requested fps
    frame = new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    frame_color= new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);
    captured_frames= new Mat [FRAMENUM_MAX];
    image.size = sizeof(XI_IMG);
    image.bp = NULL;
    image.bp_size = 0;
    buffer_size = IMAGE_WIDTH*IMAGE_HEIGHT*3;
    emit getImageSize(image.width, image.height);
    qDebug("size %d %d",image.width, image.height);
    //captured_frames = new QImage[FRAMENUM_MAX];
}

void captureThread::run(){
   xiStartAcquisition(handle);
   here:
   time_start= QDateTime::currentDateTime().toMSecsSinceEpoch();
   for (framenum=0; framenum<framenum_max; framenum++){
       xiGetImage(handle, 1000, &image); // Capture Image //timeout in microseconds
       //temp= QImage(static_cast<unsigned char*>(image.bp), IMAGE_WIDTH, IMAGE_HEIGHT, 3*IMAGE_WIDTH, QImage::Format_RGB888); //rgb24
       //temp= QImage(static_cast<unsigned char*>(image.bp), IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_WIDTH, QImage::Format_Grayscale8);
       memcpy(frame->data, image.bp, IMAGE_WIDTH*IMAGE_HEIGHT);

       // whether save or display
       if (save_frames) captured_frames[framenum]= frame->clone();
//     else if (!(frames_in_sec%RENDER_INTERVAL)) emit getImage(image.bp); // passing pointer isn't good idea as capture buffer may be changed
       else if (!(frames_in_sec%RENDER_INTERVAL)){
            cvtColor(*frame, *frame_color, CV_BayerBG2BGR);

            // color correction
            offset = 3*IMAGE_WIDTH*IMAGE_HEIGHT -1;
restofimage:
            switch(offset%3){
            case 0: // blue
                frame_color->data[offset] = frame_color->data[offset] * 2.4;
                break;
            case 1: // green
                frame_color->data[offset] = frame_color->data[offset] * 1.33;
                break;
            case 2: // red
                frame_color->data[offset] = frame_color->data[offset] * 1.43;
                break;
            default:
                break;
            }
            offset--;
            if (offset>-1) goto restofimage;


            assert(frame_color->isContinuous()); // make sure the memory is contiguous
            temp = QImage(frame_color->data, frame_color->cols, frame_color->rows, frame_color->cols*3, QImage::Format_RGB888);
            emit getImage(temp);
        }

       // timing, 1 s interval
       time_stop = QDateTime::currentDateTime().toMSecsSinceEpoch();
       time_lapsed= time_stop- time_start;
       if (time_lapsed > 1000){ // 1 sec lapsed
            emit getFPS(frames_in_sec);
            qDebug("fps-counter %d",frames_in_sec);
            time_start= time_stop;
            frames_in_sec= 0;
            //xiGetParamFloat(handle, XI_PRM_FRAMERATE, &fps); //that fps thing
            //qDebug("fps-board %f",fps);

       }
       else frames_in_sec++;
       if (!save_frames) framenum= 0;
   }

   if (save_frames){
   for (int framenum=0; framenum<framenum_max; framenum++){
        qDebug("saving %d",framenum);
        cvtColor(captured_frames[framenum], *frame_color, CV_BayerBG2BGR);
        // color correction
        offset = 3*IMAGE_WIDTH*IMAGE_HEIGHT -1;
restofimage:
        switch(offset%3){
        case 0: // blue
            frame_color->data[offset] = frame_color->data[offset] * 2.4;
            break;
        case 1: // green
            frame_color->data[offset] = frame_color->data[offset] * 1.33;
            break;
        case 2: // red
            frame_color->data[offset] = frame_color->data[offset] * 1.43;
            break;
        default:
            break;
        }
        offset--;
        if (offset>-1) goto restofimage;
        //opencv saving is faster
        sprintf(filename,"xi%04d.png",framenum);
        imwrite(filename, *frame_color);
    }
   save_frames= FALSE;
   goto here;
   }

   //exit(0);
    //this->start(QThread::NormalPriority);
}

void captureThread::saveFrames(){
    save_frames= TRUE;
    framenum= 0;
}

void captureThread::setFramesToSave(int val){
    if (val>FRAMENUM_MAX) framenum_max= FRAMENUM_MAX;
    else framenum_max= val;
    qDebug("saving buffer= %d", framenum_max);
}

void captureThread::setExposure(int val){
    xiSetParamInt(handle, XI_PRM_EXPOSURE, val); // us
    qDebug("set exposure to %d",val);
}
