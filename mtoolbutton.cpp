#include "mtoolbutton.h"

MToolButton::MToolButton(QWidget *parent) :
    QToolButton(parent)
{

}

void MToolButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::RightButton)
    {
        emit rightClicked();
    }
    if(e->button()==Qt::LeftButton)
    {
        emit clicked();
    }
}
