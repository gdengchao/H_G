#include "runningmsgwidget.h"
#include "ui_runningmsgwidget.h"

RunningMsgWidget::RunningMsgWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RunningMsgWidget)
{
    ui->setupUi(this);
    this->installEventFilter(this);
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
    qDebug() << "RunningMsgWidget::closeEvent";

    if (this->isVisible())
    {
        emit this->closeSignal();
    }
    event->ignore();
//    if (this->isVisible())
//    {
//        QMessageBox::StandardButton ret = QMessageBox::information(this, "Notice",
//            "The association will be terminated if close the widget!   ", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
//        if (ret == QMessageBox::Yes)
//        {
//            emit this->closeSignal();
//            this->clearText();
//            event->accept();
//        }
//        else
//        {
//            event->ignore();
//        }
//    }
}

//bool RunningMsgWidget::eventFilter(QObject *object, QEvent *event)
//{
//    if (event->type() == QEvent::Close)
//    if (object != this)
//    {
//        return true;
//    }
//    return false;
//}

// Delete the last line and append the new line.
void RunningMsgWidget::refreshLastLine(QString line)
{
    if (line.isNull())
    {
        return;
    }

    int index = line.length();
    if (line[index-1] == '\n' || line[index-1] == '\r')
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
