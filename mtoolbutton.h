#ifndef MTOOLBUTTON_H
#define MTOOLBUTTON_H

#include <QObject>
#include <QToolButton>
#include <QAction>
#include <QList>

class MToolButton : public QToolButton
{

    Q_OBJECT
public:
    explicit MToolButton(QWidget *parent = nullptr);

protected slots:
    void mousePressEvent(QMouseEvent *e);

signals:
    void rightClicked();
};

#endif // MTOOLBUTTON_H
