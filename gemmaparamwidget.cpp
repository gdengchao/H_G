#include "gemmaparamwidget.h"
#include "ui_gemmaparamwidget.h"

GemmaParamWidget::GemmaParamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GemmaParamWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("Detail settings");

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

GemmaParamWidget::~GemmaParamWidget()
{
    delete ui;
    delete kinAutoBtnGroup;
    delete kinMatrixBtnGroup;
}

bool GemmaParamWidget::isMakeKinAuto(void)
{
    if (!ui->yesKinRadioButton)
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isBNkinMatrix(void)
{
    if (!ui->bnRadioButton)
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isIBSkinMatrix(void)
{
    if (!ui->ibsRadioButton)
    {
        return false;
    }
    return true;
}

/**
 * @brief GemmaParamWidget::getCurrentParam
 * @return A QStringList contain the values in the widget.
 */
QMap<QString, QString> GemmaParamWidget::getCurrentParam(void)
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
