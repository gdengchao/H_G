#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>

class Process : public QProcess
{
    Q_OBJECT
public:
    explicit Process(QIODevice *parent = nullptr);
    QString getMessage();

public slots:
    void on_readProcessOutput();
    void on_readProcessError();

signals:
    void outMessageReady(QString);
    void errMessageReady(QString);
};

#endif // PROCESS_H
