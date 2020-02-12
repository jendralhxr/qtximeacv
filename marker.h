#ifndef MARKER_H
#define MARKER_H
#define NUM_MODES 10 // liek libeemd, log2(SAMPLE_WINDOW)
#define SAMPLE_WINDOW 512
#define LATERAL_D 0
#define VERTICAL_D 1

// handles image centroid and compenstation from reference marker
// estimate displacement
// do FRF and EMD
// import pffft (or GSL?) and libeemd here

#include <eemd.h>
#include <opencv2/opencv.hpp>
#include <QThread>

class marker : public QThread
{
    Q_OBJECT
public:
    explicit marker(QThread *parent = 0);
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
    void getFRF_lateral(double *frf);
    void getFRF_vertical(double *frf);
    void getCentroid(double x, double y);

public slots:
    void updatePosition(int x, int y);
    void calculateFFT();
    void calculateEMD();
    void calculateDamping();
    void setReference(double x, double y);
    void setScalingFactor(double lateral_sf, double vertical_sf);
    void setFrame(cv::Mat *frame);
    void setSelectedIMF(int num);
    void setBaseline();

private:
    unsigned char bin[256], bin_maximum, threshold;
    int x_start, x_end, y_start, y_end;
    double area;
    double scalingfactor[2]; // 0 is lateral, 1 is vertical
    double centroid[2], reference[2]; // centroid position offset to the reference position
    double displacement[2][SAMPLE_WINDOW];
    double zeta[2][NUM_MODES];
    double imfs[2][NUM_MODES][SAMPLE_WINDOW];
    // double speed[2][SAMPLE_WINDOW], acceleration[2][SAMPLE_WINDOW]; // unused atm
    double displacement_peak[2];
    double baseline[2]; // average over duration, or when there was no vibration
    // fft w/ gsl
    double displacement_fft[2][2*SAMPLE_WINDOW];
    double frf[2][SAMPLE_WINDOW/2];
    libeemd_error_code err;
    int selectimf;
    void run();


    // need whole run displacement log

};

#endif // MARKER_H
