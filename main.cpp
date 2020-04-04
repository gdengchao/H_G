#include "mainwindow.h"
#include <QApplication>

#include <RInside.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QString file = "/home/chao/Desktop/test/output/a.b.assoc.txt";
//    QFileInfo fileInfo(file);
//    QString baseName = fileInfo.baseName();
//    QString abPath = fileInfo.absolutePath();
//    QDir abDir = fileInfo.absoluteDir();
//    QString fileName = fileInfo.fileName();
//    QString suffix = fileInfo.suffix();
//    QString comSuffix = fileInfo.completeSuffix();
//    QString comBaseName = fileInfo.completeBaseName();

//    QProcess pro;
//    pro.start("Rscript");
//    if (pro.waitForStarted(-1))
//    {
//        qDebug() << "started." << endl;
//    }
//    else
//    {
//        qDebug() << "Not started." << endl;
//    }

    MainWindow w;
    w.show();

    return a.exec();
}
