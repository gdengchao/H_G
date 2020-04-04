#ifndef MPUSHBUTTON_H
#define MPUSHBUTTON_H

#include <QObject>
#include <QToolButton>
#include <QAction>
#include <QList>

class MPushButton : public QToolButton
{

    Q_OBJECT
public:
    MPushButton();
    MPushButton(QWidget *parent)
    ~MPushButton();
};

#endif // MPUSHBUTTON_H
