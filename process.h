#ifndef PROCESS_H
#define PROCESS_H

#include <QDebug>
#include <QProcess>

class Process : public QProcess
{
    Q_OBJECT
public:
    explicit Process(QObject *parent = nullptr);
    QString getMessage();

public slots:
    void on_readProcessOutput();
    void on_readProcessError();
    void on_closeProcess();

signals:
    void outMessageReady(QString);
    void errMessageReady(QString);
};

#endif // PROCESS_H
