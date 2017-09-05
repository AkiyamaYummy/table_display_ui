#ifndef EXTENDED_LABEL_H
#define EXTENDED_LABEL_H

#include <QLabel>
#include <QMouseEvent>

class ExQLabel :public QLabel{
    Q_OBJECT
protected:
    virtual void mousePressEvent(QMouseEvent* event);
signals:
    void clicked();
};

#endif // EXTENDED_LABEL_H
