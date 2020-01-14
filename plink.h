#ifndef PLINK_H
#define PLINK_H

#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QMessageBox>
#include "useros.h"

class Plink
{
public:
    Plink();
    bool transformFile(QString srcFileType, QString dstFileType);
    bool runGWAS(QString phenotype, QString genotype, QString map,
                 QString covariate, QString kinship, QString model,
                 QString ms, QString maf, QString out);
    QStringList getParamList(void);
    QString getParamString(void);

private:
    const QString toolpath;
    const QStringList model;
    QStringList paramlist;
};

#endif // PLINK_H
