#ifndef FUNCTIONALANNOTATOR_H
#define FUNCTIONALANNOTATOR_H

#include <QString>
#include <QtMath>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QApplication>

class FunctionalAnnotator
{
public:
    FunctionalAnnotator();
    bool filterSNP(QString pvalFilePath, QString thBase, QString thExpo, QString outPath);

};

#endif // FUNCTIONALANNOTATOR_H
