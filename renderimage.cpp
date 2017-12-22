#include "renderimage.h"
#include <stdlib.h>
#include <QLine>
#include <QLineF>

#define IMAGE_WIDTH 1024
#define IMAGE_HEIGHT 1024
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
    canvas= image;
    // setPixmap(QPixmap::fromImage(image.rgbSwapped())); // no scaling, RGB-BGR thing
    setPixmap(QPixmap::fromImage(canvas).scaledToWidth(IMAGE_WIDTH,Qt::FastTransformation)); // if color
    //setPixmap(QPixmap::fromImage(image).scaledToWidth(IMAGE_WIDTH,Qt::FastTransformation)); // if grayscale, didnt bother with RGB BGR
    painter.begin(&canvas);
    painter.setPen(qRgb(0,0,255));
    painter.drawLine(0, 0, 1000, 300);
    painter.drawLine(100, 100, 1000, 200);
    painter.end();
    update();

}

int renderImage::setFrameSize(unsigned width, unsigned height){
    frame_width= width;
    frame_height= height;
    return(width-height);
}

void renderImage::drawMarkerLine(int x0, int y0, int x1, int y1){
    /*painter.begin(&canvas);
    painter.setPen(qRgb(0,255,0));
    painter.drawLine(x0, y0, x1, y1);
    painter.end();
    update();*/
}
