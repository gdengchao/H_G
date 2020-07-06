#ifndef QUALITYCTRLWIDGET_H
#define QUALITYCTRLWIDGET_H

#include <QWidget>

namespace Ui {
class QualityCtrlWidget;
}

class QualityCtrlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QualityCtrlWidget(QWidget *parent = nullptr);
    ~QualityCtrlWidget();

private:
    Ui::QualityCtrlWidget *ui;
};

#endif // QUALITYCTRLWIDGET_H
