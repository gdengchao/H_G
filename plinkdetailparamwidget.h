#ifndef PLINKDETAILPARAMWIDGET_H
#define PLINKDETAILPARAMWIDGET_H

#include <QWidget>

namespace Ui {
class PlinkDetailParamWidget;
}

class PlinkDetailParamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlinkDetailParamWidget(QWidget *parent = nullptr);
    ~PlinkDetailParamWidget();

private:
    Ui::PlinkDetailParamWidget *ui;
};

#endif // PLINKDETAILPARAMWIDGET_H
