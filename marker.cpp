#include "marker.h"
#include <math.h>
#define PI 3.1415926

marker::marker(QObject *parent) : QObject(parent){

}

void marker::setROI(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1){
    x_start= x0;
    x_end= x1;
    y_start= y0;
    y_end= y1;
}

int marker::getMaximumIntensity(){
    return(bin_maximum);
}

double marker::getAverageIntensity(){
    double temp=0, count=0;
    for (int i=threshold; i<256; i++){
        temp+= bin[i]*i;
        count+= bin[i];
    }
    return(temp/count);
}

double marker::getCircleWidth(){
    return(2*sqrt(area/PI));
}

int marker::getVerticalWidth(){
    // to be decided
    return(y_end-y_start);
}

int marker::getHorizontalWidth(){
    // to be decided
    return(x_end-x_start);
}

void marker::addPixel(int x, int y, int val){
    // just for checking
    //if ((x>x_start) && (x<x_end) && (y>y_start) && (y<y_end)){
        bin[val]++;
        if (val>bin_maximum) bin_maximum= val;
        if (val>threshold) area++;
    //}
}

void marker::setThreshold(int val){
    threshold= val;
}

void marker::reset(){
    for (int i=0; i<256; i++) bin[i]= 0;
    area= 0;
    bin_maximum= 0;
}
