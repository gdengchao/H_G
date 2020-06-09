#include "emmaxparamwidget.h"
#include "ui_emmaxparamwidget.h"

EmmaxParamWidget::EmmaxParamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmmaxParamWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("Emmax settings");

    // Create button group and set exclusive.
    kinAutoBtnGroup = new QButtonGroup;
    kinMatrixBtnGroup = new QButtonGroup;
    kinAutoBtnGroup->addButton(ui->yesKinRadioButton);
    kinAutoBtnGroup->addButton(ui->noKinRadioButton);
    kinAutoBtnGroup->setExclusive(true);
    kinMatrixBtnGroup->addButton(ui->bnRadioButton);
    kinMatrixBtnGroup->addButton(ui->ibsRadioButton);
    kinMatrixBtnGroup->setExclusive(true);

    // Set default paramters.
    ui->yesKinRadioButton->setChecked(true);
    ui->bnRadioButton->setChecked(true);
}

EmmaxParamWidget::~EmmaxParamWidget()
{
    delete ui;
    delete kinAutoBtnGroup;
    delete kinMatrixBtnGroup;
}

bool EmmaxParamWidget::isMakeKinAuto(void)
{
    if (!ui->yesKinRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool EmmaxParamWidget::isBNkinMatrix(void)
{
    if (!ui->bnRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool EmmaxParamWidget::isIBSkinMatrix(void)
{
    if (!ui->ibsRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

/**
 * @brief EmmaxParamWidget::getCurrentParam
 * @return A QStringList contain the values in the widget.
 *      QMap retParam: "makekin", "kinmatrix"
 *
 */
QMap<QString, QString> EmmaxParamWidget::getCurrentParam(void)
{
//    QStringList ret;
    QMap<QString, QString> retParam;

    if (this->isMakeKinAuto())
    {
        retParam.insert("makekin", "yes");
    }
    else
    {
        retParam.insert("makekin", "no");
    }

    if (this->isBNkinMatrix())
    {
        retParam.insert("kinmatrix", "BN");
    }
    else
    {
        retParam.insert("kinmatrix", "IBS");
    }

    return retParam;
}

void EmmaxParamWidget::on_noKinRadioButton_clicked()
{
    ui->ibsRadioButton->setEnabled(false);
    ui->bnRadioButton->setEnabled(false);
}

void EmmaxParamWidget::on_yesKinRadioButton_clicked()
{
    ui->ibsRadioButton->setEnabled(true);
    ui->bnRadioButton->setEnabled(true);
}
