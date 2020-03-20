#ifndef GEMMAPARAMWIDGET_H
#define GEMMAPARAMWIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QStringList>
#include <QMap>

namespace Ui {
class GemmaParamWidget;
}

class GemmaParamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GemmaParamWidget(QWidget *parent = nullptr);
    ~GemmaParamWidget();

    bool isMakeKinAuto(void);
    bool isBNkinMatrix(void);
    bool isIBSkinMatrix(void);
    QMap<QString, QString> getCurrentParam(void);

private:
    Ui::GemmaParamWidget *ui;

    QButtonGroup *kinAutoBtnGroup;
    QButtonGroup *kinMatrixBtnGroup;
};

#endif // GEMMAPARAMWIDGET_H
