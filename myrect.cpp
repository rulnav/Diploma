#include <QApplication>
#include <QtGui>
#include <QWidget>
#include <myrect.h>





SomeWidget::SomeWidget()
{
    QPalette palette(SomeWidget::palette());
    palette.setColor(backgroundRole(), Qt::white);
    setPalette(palette);
}

void SomeWidget::paintEvent(QPaintEvent *)
{
    makeGrid();
}

void SomeWidget::setColor()
{
    QPainter painter(this);
    QBrush myBrush(Qt::SolidPattern);
    QColor myColor(this->x);
    myBrush.setColor(myColor);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(1, 2, 40, 40, myBrush);
    painter.fillRect(41,42, 40, 40, myBrush);
}

void SomeWidget::makeGrid()
{
    int i = 0;
    int j = 0;
    QPainter painter(this);
    QBrush myBrush(Qt::SolidPattern);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor myColor;

    for (i = 0; i < 16; i++)
    {
        for (j = 0; j < 12; j++)
        {
//            this->colorArray[j][i] = 0xff0000;

            myColor = this->colorArray[i][j];
            myBrush.setColor(myColor);
            painter.fillRect( i * 40 , j * 40 , 40 , 40 , myBrush );
        }
    }
    update();
}

void SomeRect()
{
    SomeWidget widget;
    widget.x = 0x000001;
    widget.show();
}
