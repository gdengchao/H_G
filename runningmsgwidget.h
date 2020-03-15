#ifndef RUNNINGMSGWIDGET_H
#define RUNNINGMSGWIDGET_H

#include <QDebug>
#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class RunningMsgWidget;
}

class RunningMsgWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RunningMsgWidget(QWidget *parent = nullptr);
    ~RunningMsgWidget();
    void setText(QString text);
    void appendText(QString text);
    void clearText(void);
    void setTitle(QString title);
    void refreshLastLine(QString line);
    QString getText(void);

    // To stop process.
    void closeEvent(QCloseEvent *event);

private:
    Ui::RunningMsgWidget *ui;

signals :
    void closeSignal(void);
};

#endif // RUNNINGMSGWIDGET_H
