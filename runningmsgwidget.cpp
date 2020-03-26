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
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void RunningMsgWidget::appendText(QString text)
{
    ui->textBrowser->append(text);
    ui->textBrowser->moveCursor(QTextCursor::End);
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

void RunningMsgWidget::closeEvent(QCloseEvent *event)
{
    if (this->isVisible())
    {
        this->hide();
    }
    this->clearText();
    emit this->closeSignal();
    event->ignore();
}

// Delete the last line and append the new line.
void RunningMsgWidget::refreshLastLine(QString line)
{
    if (line.isNull())
    {
        return;
    }

    int index = line.length();
    if (line[index-1] == "\n")
    {
        index --;
    }

    if (line[index-1] != '%' || (line[index-1]=='%'&&line[index-2]=='0'&&!line[index-3].isNumber()))
    {
        this->appendText(line);
    }
    else
    {
        QStringList curText = this->getText().split("\n");
        curText.removeLast();
        curText.append(line);
        this->setText(curText.join("\n"));
    }

}
