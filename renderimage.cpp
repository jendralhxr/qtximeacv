#include "renderimage.h"
#include <stdlib.h>
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 640
#define COLOR

renderImage::renderImage(QWidget *parent) : QLabel(parent){
    //canvas =  new QImage(IMAGE_WIDTH,IMAGE_HEIGHT,QImage::Format_RGB32);
    //canvas->fill(IMAGE_WIDTH*IMAGE_HEIGHT);
    //setPixmap(QPixmap::fromImage(*canvas));
    setMaximumSize(IMAGE_WIDTH, IMAGE_HEIGHT);
}

void renderImage::receiveBitmap(void *buffer){
    setPixmap(QPixmap::fromImage(QImage(static_cast<unsigned char*>(buffer), \
       IMAGE_WIDTH, IMAGE_HEIGHT, 3*IMAGE_WIDTH, QImage::Format_RGB888).rgbSwapped()));
    update();
}

void renderImage::receiveBitmap(QImage image){
//    setPixmap(QPixmap::fromImage(image.rgbSwapped())); // no scaling
    setPixmap(QPixmap::fromImage(image).scaledToWidth(IMAGE_WIDTH,Qt::FastTransformation)); // if color
    //setPixmap(QPixmap::fromImage(image).scaledToWidth(IMAGE_WIDTH,Qt::FastTransformation)); // if grayscale, didnt bother with RGB BGR
    update();
}

int renderImage::setFrameSize(unsigned width, unsigned height){
    frame_width= width;
    frame_height= height;
    return(width-height);
}
