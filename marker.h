#ifndef MARKER_H
#define MARKER_H

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
    int getVerticalWidth();
    int getHorizontalWidth();
    void addPixel(int x, int y, int val);
    void setThreshold(int val);
    void reset();

signals:

public slots:

private:
    unsigned char bin[256], bin_maximum, threshold;
    int x_start, x_end, y_start, y_end;
    double area;

};

#endif // MARKER_H
