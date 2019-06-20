#ifndef MARKER_H
#define MARKER_H
#define NUM_MODES 11 // liek libeemd
#define SAMPLE_WINDOW 512
#define LATERAL_D 0
#define VERTICAL_D 1
// import pffft and libeemd here
#include "libeemd/eemd.h"
#include "meow_fft/meow_fft.h"

#include <QObject>

class marker : public QObject
{
    Q_OBJECT
public:
    explicit marker(QObject *parent = 0);
    void setROI(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1);
    int getMaximumIntensity();
    double getAverageIntensity();
    double getCircleWidth();
    double getCenterX();
    double getCenterY();
    int getVerticalWidth();
    int getHorizontalWidth();
    void addPixel(int x, int y, int val);
    void setThreshold(int val);
    void reset();

signals:

public slots:
    void setBaseLine(int x, int y);
    void setBaseLine();
    void updatePosition(int x, int y);

private:
    unsigned char bin[256], bin_maximum, threshold;
    int x_start, x_end, y_start, y_end;
    double area;
    double frequency_response[2][SAMPLE_WINDOW];
    double displacement[2][SAMPLE_WINDOW];
    double zeta[2][NUM_MODES];
    double imfs[2][NUM_MODES][SAMPLE_WINDOW];
    double speed[2][SAMPLE_WINDOW], acceleration[2][SAMPLE_WINDOW]; // unused atm
    double displacement_peak[2];
    double baseline[2];

};

#endif // MARKER_H
