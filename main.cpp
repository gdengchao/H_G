#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*
     * To know file info
     */
//    QString file = "/home/chao/Desktop/test/output/a.b.assoc.txt";
//    QFileInfo fileInfo(file);
//    QString baseName = fileInfo.baseName();
//    QString abPath = fileInfo.absolutePath();
//    QDir abDir = fileInfo.absoluteDir();
//    QString fileName = fileInfo.fileName();
//    QString suffix = fileInfo.suffix();
//    QString comSuffix = fileInfo.completeSuffix();
//    QString comBaseName = fileInfo.completeBaseName();

    /*
     *  To test open external tool
     */
//    QProcess pro;
//    pro.start("Rscript");
//    if (pro.waitForStarted(-1))
//    {
//        qDebug() << "started." <<endl;
//    }
//    else
//    {
//        qDebug() << "Not started." << endl;
//    }

    /*
     *  To test QRegExp
     */
//    QString str = "HDH_G00730plitHDH_T00730:exon1:c.A132G:p.T44T,";
//    QRegExp regExp("([^:]*):(.*),");
//    int pos = regExp.indexIn(str);
//    QStringList list = regExp.capturedTexts();
//    QString strA = regExp.cap(1);
//    QString strB = regExp.cap(2);

//    QString str("A(B)");
//    QStringList gene = str.split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);

    MainWindow w;
    w.show();

    return a.exec();
}
