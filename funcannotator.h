#ifndef FUNCTIONALANNOTATOR_H
#define FUNCTIONALANNOTATOR_H

#include <QString>
#include <QtMath>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QApplication>
#include <QMap>
#include <iostream>

// Functional annotator
class FuncAnnotator
{
public:
    FuncAnnotator();
    bool filterSNP(QString const pvalFilePath, QString const thBase, QString const thExpo, QString const outFilePath);
    bool extractPos(QString const pvalFilePath, QString const mapFilePath, QString const outFilePath);
    bool complExoSnpInfo(QString const snpPosFilePath, QString const exValFuncFilePath, QString const outFilePath);
};

#endif // FUNCTIONALANNOTATOR_H
