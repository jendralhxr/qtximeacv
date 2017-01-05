#include "renderimage.h"
#include <stdlib.h>
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

renderImage::renderImage(QWidget *parent) : QLabel(parent){
    //canvas =  new QImage(IMAGE_WIDTH,IMAGE_HEIGHT,QImage::Format_RGB32);
    //canvas->fill(IMAGE_WIDTH*IMAGE_HEIGHT);
    //setPixmap(QPixmap::fromImage(*canvas));
    setMinimumHeight(IMAGE_HEIGHT);
    setMinimumWidth(IMAGE_WIDTH);
}

void renderImage::receiveBitmap(void *buffer){
    setPixmap(QPixmap::fromImage(QImage(static_cast<unsigned char*>(buffer), \
       IMAGE_WIDTH, IMAGE_HEIGHT, 3*IMAGE_WIDTH, QImage::Format_RGB888).rgbSwapped()));
    update();

}

void renderImage::receiveBitmap(QImage image){
    setPixmap(QPixmap::fromImage(image.rgbSwapped()));
    update();
}

int renderImage::setFrameSize(unsigned width, unsigned height){
    frame_width= width;
    frame_height= height;
    return(width-height);
}
