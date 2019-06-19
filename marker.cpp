#include "marker.h"
#include <math.h>
#include <stdlib.h>

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
        bin[val]++;
        if (val>bin_maximum) bin_maximum= val;
        if (val>threshold) area++;
        if (val>threshold){
            if (x > x_end) x_end= x;
            if (x < x_start) x_start= x;
            if (y > y_end) y_end= y;
            if (y < y_start) y_start= y;
        }
}

void marker::setThreshold(int val){
    threshold= val;
}

void marker::reset(){
    for (int i=0; i<256; i++) bin[i]= 0;
    area= 0;
    bin_maximum= 0;
    x_start= 2048;
    x_end= 0;
    y_start = 2048;
    y_end = 0;
}

double marker::getCenterX(){
    return ((x_start+x_end)/2);
    // or any relevant centroid calculation
}

double marker::getCenterY(){
    return ((y_start+y_end)/2);
    // or any relevant centroid calculation
}

void marker::setBaseLine(int x, int y){
    baseline[LATERAL_D]= x;
    baseline[VERTICAL_D]= y;
}

void marker::setBaseLine(){
    baseline[LATERAL_D]= displacement[LATERAL_D][SAMPLE_WINDOW-1];
    baseline[VERTICAL_D]= displacement[VERTICAL_D][SAMPLE_WINDOW-1];
}

void marker::updatePosition(int x, int y){
    memmove(&(displacement[LATERAL_D][0]), &(displacement[LATERAL_D][1]), sizeof(double)*(SAMPLE_WINDOW-1));
    memmove(&(displacement[VERTICAL_D][0]), &(displacement[VERTICAL_D][1]), sizeof(double)*(SAMPLE_WINDOW-1));
    displacement[LATERAL_D][SAMPLE_WINDOW-1]= x;
    displacement[VERTICAL_D][SAMPLE_WINDOW-1]= y;

// calculate fft, emd

}
