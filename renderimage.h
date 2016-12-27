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

public slots:
    void receiveBitmap(void *buffer);
    void receiveBitmap (QImage image);
    void receiveBitmapRaw (QImage image);
    int setFrameSize(unsigned width, unsigned height);

private:
    unsigned frame_width, frame_height;
};

#endif // RENDERIMAGE_H
