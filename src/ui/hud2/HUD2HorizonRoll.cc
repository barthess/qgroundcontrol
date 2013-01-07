#include <math.h>

#include <QtGui>

#include "HUD2Math.h"
#include "HUD2HorizonRoll.h"

HUD2HorizonRoll::HUD2HorizonRoll(const HUD2Data *huddata, QWidget *parent) :
    QWidget(parent),
    huddata(huddata)
{
    this->thickPen = QPen(Qt::green);
    this->thickPen.setWidth(4);

    this->thinPen = QPen(Qt::green);
    this->thinPen.setWidth(1);

    this->arrowPen = QPen(Qt::green);
    this->arrowPen.setWidth(2);
}

void HUD2HorizonRoll::updateGeometry(const QSize *size){
    int thick_scratch_len = size->height() / 40;
    putinrange(thick_scratch_len, 4, 20);

    int thin_scratch_len = thick_scratch_len / 2;
    putinrange(thin_scratch_len, 2, 10);

    int big_r = size->height() / 2;
    int small_r = big_r - thick_scratch_len;

    QLine thick_line = QLine(0, big_r, 0, small_r);
    QLine thin_line = QLine(0, small_r, 0, small_r + thin_scratch_len);

    // big scratches
    int phi_step = 30;
    int phi = -60;
    int i = 0;
    int n = 0;

    n = sizeof(thickLines) / sizeof(thickLines[0]);
    for (i=0; i<n; i++){
        thickLines[i] = rotateLine(phi, thick_line);
        phi += phi_step;
    }

    // small scratches
    phi_step = 10;
    phi = -80;
    n = sizeof(thinLines) / sizeof(thinLines[0]);
    i = 0;
    while (i < n){
        if ((phi % 30) != 0){
            thinLines[i] = rotateLine(phi, thin_line);
            i++;
        }
        phi += phi_step;
    }

    // arrow
    QPoint p0 = QPoint(0, small_r); // top
    QPoint p1 = p0;
    QPoint p2 = p0;
    p1.rx() += 30;
    p1.ry() -= 30;
    p2.rx() -= 30;
    p2.ry() -= 30;
    arrowLines[0] = QLine(p0, p1);
    arrowLines[1] = QLine(p0, p2);
}

void HUD2HorizonRoll::paint(QPainter *painter, QColor color){
    Q_UNUSED(color);
    int n = 0;

    painter->save();

    painter->setPen(thickPen);
    n = sizeof(thickLines) / sizeof(thickLines[0]);
    painter->drawLines(thickLines, n);

    painter->setPen(thinPen);
    n = sizeof(thinLines) / sizeof(thinLines[0]);
    painter->drawLines(thinLines, n);

    n = sizeof(arrowLines) / sizeof(arrowLines[0]);
    painter->setPen(arrowPen);
    painter->rotate(-rad2deg(huddata->roll));
    painter->drawLines(arrowLines, n);

    painter->restore();
}

void HUD2HorizonRoll::setColor(QColor color){
    Q_UNUSED(color);
}