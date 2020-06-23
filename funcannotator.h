#ifndef FUNCTIONALANNOTATOR_H
#define FUNCTIONALANNOTATOR_H

#include <QString>
#include <QtMath>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QApplication>
#include <QMap>
#include <QSet>
#include <iostream>

// Functional annotator
class FuncAnnotator
{
public:
    FuncAnnotator();
    bool filterSNP(QString const pvalFilePath, QString const thBase, QString const thExpo, QString const outFilePath);
    bool extractPos(QString const pvalFilePath, QString const mapFilePath, QString const outFilePath);
    bool complExoSnpInfo(QString const snpPosFilePath, QString const exVarFuncFilePath, QString const outFilePath);
    bool complNonExoSnpInfo(QString const exonicPosFilePath, QString const snpPosFilePath,
                            QString const varFuncFilePath, QString const outFilePath);
    bool complFuncAnnoInfo(QString const exonicPosFilePath, QString nonExonicPosFilePath,
                           QString baseFilePath, QString outFilePath);
};

#endif // FUNCTIONALANNOTATOR_H
