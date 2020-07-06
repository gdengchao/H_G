#include "qualityctrlwidget.h"
#include "ui_qualityctrlwidget.h"

QualityCtrlWidget::QualityCtrlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QualityCtrlWidget)
{
    ui->setupUi(this);
}

QualityCtrlWidget::~QualityCtrlWidget()
{
    delete ui;
}
