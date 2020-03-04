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
    bool vcf2transpose(QString vcfFile, QString out, QString maf, QString ms);
    bool vcf2binary(QString vcfFile, QString out, QString maf, QString ms);
    bool plink2transpose(QString pedFile, QString mapFile, QString out, QString maf, QString ms);
    bool plink2binary(QString pedFile, QString mapFile, QString out);
    bool runGWAS(QString phenotype, QString genotype, QString map,
                 QString covariate, QString kinship, QString model,
                 QString ms, QString maf, QString out);
    QStringList getParamList(void);    // Every param can't have any space.
    QString getParamString(void);

private:
    const QString toolpath;
    const QStringList model;
    QStringList paramlist;
};

#endif // PLINK_H
