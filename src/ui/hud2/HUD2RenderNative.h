#ifndef HUD2RENDERNATIVE_H
#define HUD2RENDERNATIVE_H

#include <QWidget>
#include "HUD2Data.h"
#include "HUD2Painter.h"

class HUD2Painter;
QT_BEGIN_NAMESPACE
class QPaintEvent;
QT_END_NAMESPACE

class HUD2RenderNative : public QWidget
{
    Q_OBJECT

public:
    HUD2RenderNative(HUD2Data &huddata, QWidget *parent);

signals:
    void geometryChanged(const QSize *size);

public slots:
    void paint(void);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    HUD2Painter hudpainter;
};

#endif /* HUD2RENDERNATIVE_H */