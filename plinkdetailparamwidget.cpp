#include "plinkdetailparamwidget.h"
#include "ui_plinkdetailparamwidget.h"

PlinkDetailParamWidget::PlinkDetailParamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlinkDetailParamWidget)
{
    ui->setupUi(this);
}

PlinkDetailParamWidget::~PlinkDetailParamWidget()
{
    delete ui;
}
