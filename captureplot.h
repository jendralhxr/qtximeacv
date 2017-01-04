#ifndef CAPTUREPLOT_H
#define CAPTUREPLOT_H

#define SAMPLECOUNT 100
#include <QWidget>
#include <QColor>
#include <QDateTime>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class capturePlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit capturePlot(QWidget *parent = 0);
 //   capturePlot();

public slots:
    void updatePoints(double val);

private:
    QwtPlotCurve curve;
    double dataAxis[SAMPLECOUNT], timeAxis[SAMPLECOUNT];
};

#endif // CAPTUREPLOT_H
