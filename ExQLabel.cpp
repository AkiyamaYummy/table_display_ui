#include "ExQLabel.h"

void ExQLabel::mousePressEvent(QMouseEvent* event){
    if (event->button() == Qt::LeftButton)
        emit clicked();
    QLabel::mousePressEvent(event);
}
