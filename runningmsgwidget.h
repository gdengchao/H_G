#ifndef RUNNINGMSGWIDGET_H
#define RUNNINGMSGWIDGET_H

#include <QWidget>

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
    QString getText(void);
private:
    Ui::RunningMsgWidget *ui;
};

#endif // RUNNINGMSGWIDGET_H
