#ifndef MYRECT_H
#define MYRECT_H

#include <QApplication>
#include <QtGui>
#include <QWidget>
#include <myrect.h>

class SomeWidget : public QWidget
{
    public:
        SomeWidget();
        int x;
        int colorArray[16][12];
        void setColor();
        void makeGrid();
    protected:
        void paintEvent(QPaintEvent *event);
};

#endif // MYRECT_H
