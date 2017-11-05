#include "capturethread.h"
#include <string.h>
#include <QDateTime>

#define RENDER_INTERVAL 10
#define IMAGE_WIDTH 2048
#define IMAGE_HEIGHT 752//752
#define EXPOSURE 2000 // us
#define FPS_REQUESTED 240
#define FRAMENUM_MAX 3000
// pick one of these two
//#define HEAD_COLOR
#define HEAD_MONOCHROME

using namespace cv;

char filename[20];
int timestamp_temp;
float min_fps, max_fps;

/*
captureThread::captureThread(QObject *parent) : QThread(parent){
}*/

captureThread::captureThread() // no argument
{

    framenum_max= FRAMENUM_MAX;
    xiSetParamFloat(handle, XI_PRM_GAIN, 7.4); // -3.5 to 7.4
    //xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24); // simply cause I can
    xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RAW8); // faster
    xiSetParamInt(handle, XI_PRM_OFFSET_X, 0);
    xiSetParamInt(handle, XI_PRM_OFFSET_Y, 0); // 824
    xiSetParamInt(handle, XI_PRM_WIDTH, IMAGE_WIDTH);
    xiSetParamInt(handle, XI_PRM_HEIGHT, IMAGE_HEIGHT);
    xiSetParamInt(handle, XI_PRM_EXPOSURE, EXPOSURE); // us
    //xiSetParamInt(handle, XI_PRM_AUTO_WB, 0);
    // simply desperate to get it dialed in

//    xiSetParamInt(handle, XI_PRM_GPO_SELECTOR, 1);
 //   xiSetParamInt(handle, XI_PRM_GPO_MODE,XI_GPO_EXPOSURE_PULSE);

    //xiSetParamInt(handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FREE_RUN );// maximum frame rate
    xiSetParamInt(handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);// set acquisition to frame rate mode
//    xiSetParamInt(handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);// set acquisition to frame rate mode
    xiSetParamInt(handle, XI_PRM_FRAMERATE, FPS_REQUESTED);// Requested fps`
//    xiSetParamInt(handle, XI_PRM_TRG_SOURCE, XI_TRG_EDGE_RISING);// maximum frame rateXI_TRG_SEL_FRAME_START

    frame = new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    frame_buffer= new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    frame_color= new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);

    captured_frames= new Mat [FRAMENUM_MAX];
    timestamp = new struct timeval [FRAMENUM_MAX];
    image.size = sizeof(XI_IMG);
    image.bp = NULL;
    image.bp_size = 0;
    //buffer_size = IMAGE_WIDTH*IMAGE_HEIGHT*3;
    emit getImageSize(image.width, image.height);
    qDebug("size %d %d",image.width, image.height);
    //captured_frames = new QImage[FRAMENUM_MAX];

    logfile =  new QFile();
    logfile->setFileName("/me/timing.txt");
    streamout = new QTextStream(logfile);
}

captureThread::captureThread(int dev){
    devicenum= dev;
    qDebug("device num %d",devicenum);
    if(xiOpenDevice(devicenum, &handle) != XI_OK) exit(1); // device num
    framenum_max= FRAMENUM_MAX;
    xiSetParamFloat(handle, XI_PRM_GAIN, 7.4); // -3.5 to 7.4
    //xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB24); // simply cause I can
    xiSetParamInt(handle, XI_PRM_IMAGE_DATA_FORMAT, XI_RAW8); // faster
    xiSetParamInt(handle, XI_PRM_OFFSET_X, 0);
    xiSetParamInt(handle, XI_PRM_OFFSET_Y, 0); // 824
    xiSetParamInt(handle, XI_PRM_WIDTH, IMAGE_WIDTH);
    xiSetParamInt(handle, XI_PRM_HEIGHT, IMAGE_HEIGHT);
    xiSetParamInt(handle, XI_PRM_EXPOSURE, EXPOSURE); // us
    //xiSetParamInt(handle, XI_PRM_AUTO_WB, 0);
    // simply desperate to get it dialed in

    xiSetParamInt(handle, XI_PRM_GPO_SELECTOR, 1);
    xiSetParamInt(handle, XI_PRM_GPO_MODE,XI_GPO_EXPOSURE_PULSE);

    xiSetParamInt(handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FREE_RUN );// maximum frame rate
    //xiSetParamInt(handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);// set acquisition to frame rate mode
    //xiSetParamInt(handle, XI_PRM_FRAMERATE, FPS_REQUESTED);// Requested fps`
    //xiSetParamInt(handle, XI_PRM_TRG_SOURCE, XI_TRG_EDGE_RISING);// maximum frame rateXI_TRG_SEL_FRAME_START

    frame = new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    frame_buffer= new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
    frame_color= new Mat(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);

    captured_frames= new Mat [FRAMENUM_MAX];
    timestamp = new struct timeval [FRAMENUM_MAX];
    image.size = sizeof(XI_IMG);
    image.bp = NULL;
    image.bp_size = 0;
    //buffer_size = IMAGE_WIDTH*IMAGE_HEIGHT*3;
    emit getImageSize(image.width, image.height);
    qDebug("size %d %d",image.width, image.height);
    //captured_frames = new QImage[FRAMENUM_MAX];

    logfile =  new QFile();
    logfile->setFileName("/me/timing.txt");
    streamout = new QTextStream(logfile);
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

        // passing pointer to image.bp isn't good idea as capture buffer may be changed
        if (save_frames) { // whether save or display
            captured_frames[framenum]= frame->clone();
            //gettimeofday(&timestamp[framenum],NULL);
            timestamp[framenum].tv_sec= image.tsSec;
            timestamp[framenum].tv_usec= image.tsUSec;

        }

#ifdef HEAD_MONOCHROME
        // render grayscale
        else if (!(frames_in_sec%RENDER_INTERVAL)) {
            assert(frame_color->isContinuous()); // make sure the memory is contiguous
            temp = QImage(frame->data, frame->cols, frame->rows, frame->cols, QImage::Format_Grayscale8);
            emit getImage(temp);
        }
#endif

#ifdef HEAD_COLOR
        // render color
        else if (!(frames_in_sec%RENDER_INTERVAL)){
            cvtColor(*frame, *frame_buffer, CV_BayerGB2GRAY);
            //cvtColor(*frame, *frame_buffer, CV_BayerBG2GRAY);
            cvtColor(*frame_buffer, *frame_color, CV_GRAY2RGB);
            // color correction
            /*
            offset = 3*IMAGE_WIDTH*IMAGE_HEIGHT -1;
restofimage:
            switch(offset%3){
            case 0: // blue
                frame_color->data[offset] = char(frame_color->data[offset] * 2.4);
                break;
            case 1: // green
                frame_color->data[offset] = char(frame_color->data[offset] * 1.33);
                break;
            case 2: // red
                frame_color->data[offset] = char(frame_color->data[offset] * 1.43);
                break;
            default:
                break;
            }
            offset--;
            if (offset>-1) goto restofimage;
            */
            //assert(frame_color->isContinuous()); // make sure the memory is contiguous
            temp = QImage(frame_color->data, IMAGE_WIDTH, IMAGE_HEIGHT, 3*IMAGE_WIDTH, QImage::Format_RGB888);
            emit getImage(temp);
        }
#endif

        if (!save_frames) {
            // timing, 1 s interval
            xiGetParamFloat(handle, XI_PRM_FRAMERATE, &fps); //that fps thing
         //   xiGetParamInt(handle, XI_PRM_SENSOR_CLOCK_FREQ_HZ, &clock_freq);
         //   qDebug("!!CCCC clock freq: %d",clock_freq);

            time_stop = QDateTime::currentDateTime().toMSecsSinceEpoch();
            time_lapsed= time_stop- time_start;
            if (time_lapsed > 1000){ // 1 sec lapsed
                emit getFPS(frames_in_sec);
                qDebug("fps-counter %d, %f",frames_in_sec, fps);
                time_start= time_stop;
                frames_in_sec= 0;
                //qDebug("fps-board %f",fps);
                //if (!save_frames) calculateCentroids();
            }
            else frames_in_sec++;
            framenum= 0;
        }
        //else qDebug("buffering %d/%d",framenum, framenum_max);
    }

    if (save_frames){
        for (int framenum=0; framenum<framenum_max; framenum++){
            qDebug("saving %d",framenum);
            sprintf(filename,"/me/xi%d%04d.tif",devicenum,framenum);

#ifdef HEAD_COLOR
            // saving frames
            cvtColor(captured_frames[framenum], *frame_buffer, CV_BayerGB2GRAY);
            /*
        // color correction
        offset = 3*IMAGE_WIDTH*IMAGE_HEIGHT -1;
saveimage:
        switch(offset%3){
        case 0: // blue
            frame_color->data[offset] = char(frame_color->data[offset] * 2.4);
            break;
        case 1: // green
            frame_color->data[offset] = char(frame_color->data[offset] * 1.33);
            break;
        case 2: // red
            frame_color->data[offset] = char(frame_color->data[offset] * 1.43);
            break;
        default:
            break;
        }
        offset--;
        if (offset>-1) goto saveimage;
*/

            //imwrite is faster compared to QImage.save
            imwrite(filename, *frame_buffer);
#endif

#ifdef HEAD_MONOCHROME
            imwrite(filename, captured_frames[framenum]); // grayscale camera head
#endif
            timestamp_temp = timestamp[framenum].tv_usec - timestamp[framenum-1].tv_usec;
            if (timestamp_temp<0) timestamp_temp+=1e6;
            *streamout << timestamp_temp << endl;
        }
        save_frames= FALSE;
        goto here;
    }
    logfile->close();
    //exit(0);
    if (!this->isRunning()) this->start(QThread::NormalPriority);
}

void captureThread::saveFrames(){
    save_frames= TRUE;
    framenum= 0;
    qDebug("start saving %d frames",framenum_max);
    logfile->open(QIODevice::WriteOnly | QIODevice::Text);
    this->start(QThread::HighestPriority);
}

void captureThread::setFramesToSave(int val){
    if (val>FRAMENUM_MAX) framenum_max= FRAMENUM_MAX;
    else framenum_max= val;
    qDebug("saving buffer= %d", framenum_max);
    this->start(QThread::HighestPriority);
}

void captureThread::setExposure(int val){
    xiSetParamInt(handle, XI_PRM_EXPOSURE, val); // us
    qDebug("set exposure to %d",val);
    this->start(QThread::HighestPriority);
}

int captureThread::calculateCentroids(){
    bitwise_not(*frame_buffer, invert);
    detector.detect(invert, keypoints);
    //detector.detect(image, keypoints);

    int n=0;
    for (std::vector<KeyPoint>::iterator it = keypoints.begin(); it != keypoints.end(); ++it){
        //cout << it->pt.x  << ';' << it->pt.y << endl;
        moment_x[n]= it->pt.x;
        moment_y[n]= it->pt.y;
        mass[n]= it->size;
        n++;
    } //  std::cout << ' ' << *it;

    // little sorting
    for (int i=n-1; i>=0; i--){
        for (int j=i-1; j>=0; j--){
            if (moment_x[i] > moment_x[j]){
                moment_x_temp= moment_x[i];
                moment_y_temp= moment_y[i];
                mass_temp= mass[i];
                moment_x[i]= moment_x[j];
                moment_y[i]= moment_y[j];
                mass[i]= mass[j];
                moment_x[j]= moment_x_temp;
                moment_y[j]= moment_y_temp;
                mass[j]= mass_temp;
            }
        }
    }

    for (int i=n-1; i>=0; i--){
        qDebug("%f %f\n",moment_x[i], moment_y[i]);
    }
    return(n);

}
