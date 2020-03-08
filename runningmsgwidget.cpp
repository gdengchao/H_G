#include "runningmsgwidget.h"
#include "ui_runningmsgwidget.h"

RunningMsgWidget::RunningMsgWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RunningMsgWidget)
{
    ui->setupUi(this);
    this->setTitle("Running Messages");
}

RunningMsgWidget::~RunningMsgWidget()
{
    delete ui;
}

void RunningMsgWidget::setText(QString text)
{
    ui->textBrowser->setText(text);
}

void RunningMsgWidget::appendText(QString text)
{
    ui->textBrowser->append(text);
}

void RunningMsgWidget::clearText(void)
{
    ui->textBrowser->clear();
}

void RunningMsgWidget::setTitle(QString title)
{
    this->setWindowTitle(title);
}

QString RunningMsgWidget::getText(void)
{
    return ui->textBrowser->toPlainText();
}
