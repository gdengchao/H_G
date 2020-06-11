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

/**
 * @brief RunningMsgWidget::setText
 *          (Will clear original data)
 * @param text
 */
void RunningMsgWidget::setText(QString text)
{
    ui->textBrowser->setText(text);
    ui->textBrowser->moveCursor(QTextCursor::End);  // Move cursor to end.
}

/**
 * @brief RunningMsgWidget::appendText
 *          (Will not clear original text)
 * @param text: new text
 */
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

/**
 * @brief RunningMsgWidget::getText
 * @return current text in widget.
 */
QString RunningMsgWidget::getText(void)
{
    return ui->textBrowser->toPlainText();
}

/**
 * @brief RunningMsgWidget::closeEvent
 *          overrider closeEvent function.
 * @param event
 */
void RunningMsgWidget::closeEvent(QCloseEvent *event)
{
    qDebug() << "RunningMsgWidget::closeEvent";

    if (this->isVisible())
    {
        emit this->closeSignal();
    }
    event->ignore();
}

/**
 * @brief RunningMsgWidget::refreshLastLine
 *          Deal with last line of text.
 * @param line:new message.
 */
void RunningMsgWidget::refreshLastLine(QString line)
{
    if (line.isNull())
    {   // There no new message.
        return;
    }

    int index = line.length();
    if (line[index-1] == '\n' || line[index-1] == '\r')
    {   //
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
