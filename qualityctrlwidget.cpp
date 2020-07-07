#include "qualityctrlwidget.h"
#include "ui_qualityctrlwidget.h"

QualityCtrlWidget::QualityCtrlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QualityCtrlWidget)
{
    ui->setupUi(this);

    this->linkageFileterBtnGroup = new QButtonGroup;
    this->linkageFileterBtnGroup->addButton(ui->yesLinkageFilterRadioButton);
    this->linkageFileterBtnGroup->addButton(ui->noLinkageFilterRadioButton);
    this->linkageFileterBtnGroup->setExclusive(true);
    ui->yesLinkageFilterRadioButton->setCheckable(true);
    ui->noLinkageFilterRadioButton->setCheckable(true);

    // Set default status
    ui->noLinkageFilterRadioButton->setChecked(true);
    ui->winSizeLineEdit->setEnabled(false);
    ui->stepLenLineEdit->setEnabled(false);
    ui->r2ThrLineEdit->setEnabled(false);
}

QualityCtrlWidget::~QualityCtrlWidget()
{
    delete ui;
    delete this->linkageFileterBtnGroup;
}

void QualityCtrlWidget::on_noLinkageFilterRadioButton_clicked()
{
    ui->winSizeLineEdit->setEnabled(false);
    ui->stepLenLineEdit->setEnabled(false);
    ui->r2ThrLineEdit->setEnabled(false);
}

void QualityCtrlWidget::on_yesLinkageFilterRadioButton_clicked()
{
    ui->winSizeLineEdit->setEnabled(true);
    ui->stepLenLineEdit->setEnabled(true);
    ui->r2ThrLineEdit->setEnabled(true);
}
