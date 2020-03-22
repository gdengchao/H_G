#include "gemmaparamwidget.h"
#include "ui_gemmaparamwidget.h"

GemmaParamWidget::GemmaParamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GemmaParamWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("Gemma settings");

    // Create button group and set exclusive.
    kinAutoBtnGroup = new QButtonGroup;
    kinMatrixBtnGroup = new QButtonGroup;
    lmmTestBtnGroup = new QButtonGroup;
    kinAutoBtnGroup->addButton(ui->yesKinRadioButton);
    kinAutoBtnGroup->addButton(ui->noKinRadioButton);
    kinAutoBtnGroup->setExclusive(true);
    kinMatrixBtnGroup->addButton(ui->bnRadioButton);
    kinMatrixBtnGroup->addButton(ui->ibsRadioButton);
    kinMatrixBtnGroup->setExclusive(true);
    lmmTestBtnGroup->addButton(ui->waldTestRadioButton);
    lmmTestBtnGroup->addButton(ui->likelihoodRadioButton);
    lmmTestBtnGroup->addButton(ui->scoreTestRadioButton);
    lmmTestBtnGroup->addButton(ui->allTestRadioButton);
    lmmTestBtnGroup->setExclusive(true);

    // Set default paramters.
    ui->yesKinRadioButton->setChecked(true);
    ui->bnRadioButton->setChecked(true);
    ui->waldTestRadioButton->setChecked(true);
}

GemmaParamWidget::~GemmaParamWidget()
{
    delete ui;
    delete kinAutoBtnGroup;
    delete kinMatrixBtnGroup;
}

bool GemmaParamWidget::isMakeKinAuto(void)
{
    if (!ui->yesKinRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isBNkinMatrix(void)
{
    if (!ui->bnRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isIBSkinMatrix(void)
{
    if (!ui->ibsRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isWaldTest(void)
{
    if (!ui->waldTestRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isLikelihoodRatioTest(void)
{
    if (!ui->likelihoodRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isScoreTest(void)
{
    if (!ui->scoreTestRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isAllTest(void)
{
    if (!ui->allTestRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

/**
 * @brief GemmaParamWidget::getCurrentParam
 * @return A QStringList contain the values in the widget.
 *  retParam:   makekin, kinmatrix, lmmtest
 */
QMap<QString, QString> GemmaParamWidget::getCurrentParam(void)
{
//    QStringList ret;
    QMap<QString, QString> retParam;

    // Make kinship: yes or no, BN or IBS.
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

    // LMM test: wald, likelihood ratio, score, all.
    if (this->isAllTest())
    {
        retParam.insert("lmmtest", "4");
    }
    else if (this->isWaldTest())
    {
        retParam.insert("lmmtest", "1");
    }
    else if (this->isLikelihoodRatioTest())
    {
        retParam.insert("lmmtest", "2");
    }
    else if (this->isScoreTest())
    {
        retParam.insert("lmmtest", "3");
    }

    return retParam;
}
