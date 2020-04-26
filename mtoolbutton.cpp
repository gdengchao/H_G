#include "mtoolbutton.h"
#include <QDebug>

MToolButton::MToolButton(QWidget *parent) :
    QToolButton(parent)
{
    this->showMenuFlag = false;
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->menu = new QMenu(this);
    this->closeAction = new QAction(this);
    this->closeAction->setText("Close");
    this->menu->addAction(this->closeAction);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(on_closeAction()));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showMenu(const QPoint&)));
}

void MToolButton::setShowMenuFlag(bool b)
{
    this->showMenuFlag = b;
}

bool MToolButton::isShowMenuOK(void)
{
    return this->showMenuFlag;
}

void MToolButton::showMenu(const QPoint& point)
{
    if (this->showMenuFlag)
    {
        this->menu->exec(mapToGlobal(point));
    }
}

void MToolButton::on_closeAction()
{
    qDebug () << "MToolButton::on_closeAction()";
    emit closeFileSig();
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

void MToolButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        emit doubleClicked();
    }
}
