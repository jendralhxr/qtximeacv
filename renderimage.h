#ifndef RENDERIMAGE_H
#define RENDERIMAGE_H

#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QImage>

class renderImage : public QLabel
{
    Q_OBJECT
public:
    explicit renderImage(QWidget *parent = 0);

signals:
    void getCentroid(int num, int x, int y);

public slots:
    void receiveBitmap(void *buffer);
    void receiveBitmap (QImage image);
    void drawMarkerLine(int x0, int y0, int x1, int y1);
    int setFrameSize(unsigned width, unsigned height);

protected:
    void mousePressEvent(QMouseEvent *ev) override;

private:
    unsigned frame_width, frame_height;
    QPainter painter;
    QImage canvas;
    int markernum;
};

#endif // RENDERIMAGE_H
