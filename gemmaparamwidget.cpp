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
    bslmmModelBtnGroup = new QButtonGroup;
    kinAutoBtnGroup->addButton(ui->yesKinRadioButton);
    kinAutoBtnGroup->addButton(ui->noKinRadioButton);
    kinAutoBtnGroup->setExclusive(true);
    kinMatrixBtnGroup->addButton(ui->centRelatRadioButton);
    kinMatrixBtnGroup->addButton(ui->stdRelatRadioButton);
    kinMatrixBtnGroup->setExclusive(true);
    lmmTestBtnGroup->addButton(ui->waldTestRadioButton);
    lmmTestBtnGroup->addButton(ui->likelihoodRadioButton);
    lmmTestBtnGroup->addButton(ui->scoreTestRadioButton);
    lmmTestBtnGroup->addButton(ui->allTestRadioButton);
    lmmTestBtnGroup->setExclusive(true);
    bslmmModelBtnGroup->addButton(ui->stdLinearBslmmRadioButton);
    bslmmModelBtnGroup->addButton(ui->ridgeBslmmRadioButton);
    bslmmModelBtnGroup->addButton(ui->probitBslmmRadioButton);
    bslmmModelBtnGroup->setExclusive(true);

    // Set default paramters.
    ui->yesKinRadioButton->setChecked(true);
    ui->centRelatRadioButton->setChecked(true);
    ui->waldTestRadioButton->setChecked(true);
    ui->stdLinearBslmmRadioButton->setChecked(true);
}

GemmaParamWidget::~GemmaParamWidget()
{
    delete ui;
    delete kinAutoBtnGroup;
    delete kinMatrixBtnGroup;
    delete lmmTestBtnGroup;
    delete bslmmModelBtnGroup;
}

void GemmaParamWidget::setLmmParamEnabled(bool boolean)
{
    ui->waldTestRadioButton->setEnabled(boolean);
    ui->likelihoodRadioButton->setEnabled(boolean);
    ui->scoreTestRadioButton->setEnabled(boolean);
    ui->allTestRadioButton->setEnabled(boolean);
}

void GemmaParamWidget::setBslmmParamEnabled(bool boolean)
{
    ui->stdLinearBslmmRadioButton->setEnabled(boolean);
    ui->probitBslmmRadioButton->setEnabled(boolean);
    ui->ridgeBslmmRadioButton->setEnabled(boolean);
}

bool GemmaParamWidget::isMakeRelatedMatAuto(void)
{
    if (!ui->yesKinRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isCentRelatedMat(void)
{
    if (!ui->centRelatRadioButton->isChecked())
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isStdRelatedMat(void)
{
    if (!ui->stdRelatRadioButton->isChecked())
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

bool GemmaParamWidget::isStdLinearBSLMM(void)
{
    if (!ui->stdLinearBslmmRadioButton)
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isRidgeRegreBSLMM(void)
{
    if (!ui->ridgeBslmmRadioButton)
    {
        return false;
    }
    return true;
}

bool GemmaParamWidget::isProbitBSLMM(void)
{
    if (!ui->probitBslmmRadioButton)
    {
        return false;
    }
    return true;
}


/**
 * @brief GemmaParamWidget::getCurrentParam
 * @return A QStringList contain the values in the widget.
 *  retParam:   makekin, kinmatrix, lmmtest, bslmmmodel
 */
QMap<QString, QString> GemmaParamWidget::getCurrentParam(void)
{
//    QStringList ret;
    QMap<QString, QString> retParam;

    // Make kinship: yes or no, BN or IBS.
    if (this->isMakeRelatedMatAuto())
    {
        retParam.insert("makekin", "yes");
    }
    else
    {
        retParam.insert("makekin", "no");
    }

    if (this->isCentRelatedMat())
    {   // centered relatedness matrix: 1
        retParam.insert("kinmatrix", "1");
    }
    else
    {   // standardized relatedness: 2
        retParam.insert("kinmatrix", "2");
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

    if (this->isStdRelatedMat())
    {
        retParam.insert("bslmmmodel", "1");
    }
    else if (this->isRidgeRegreBSLMM())
    {
        retParam.insert("bslmmmodel", "2");
    }
    else if (this->isProbitBSLMM())
    {
        retParam.insert("bslmmmodel", "3");
    }


    return retParam;
}

void GemmaParamWidget::on_noKinRadioButton_clicked()
{
    ui->centRelatRadioButton->setEnabled(false);
    ui->stdRelatRadioButton->setEnabled(false);
}

void GemmaParamWidget::on_yesKinRadioButton_clicked()
{
    ui->centRelatRadioButton->setEnabled(true);
    ui->stdRelatRadioButton->setEnabled(true);
}
