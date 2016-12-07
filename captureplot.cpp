#include "captureplot.h"

capturePlot::capturePlot(QWidget *parent) : QwtPlot(parent){

}

void capturePlot::updatePoints(double val){
    for(int i=0; i<(SAMPLECOUNT-1); i++){
        dataAxis[i]=dataAxis[i+1];
        }
    dataAxis[SAMPLECOUNT-1]=val;
    replot();
}
