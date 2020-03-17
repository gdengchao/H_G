#ifndef PLINK_H
#define PLINK_H

#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include "useros.h"

class Plink
{
public:
    Plink();
    bool vcf2transpose(QString vcfFile, QString out, QString maf, QString ms);
    bool vcf2binary(QString vcfFile, QString out, QString maf, QString ms);
    bool vcf2plink(QString vcfFile, QString out, QString maf, QString ms);
    bool plink2transpose(QString pedFile, QString mapFile, QString out, QString maf, QString ms);
    bool plink2binary(QString pedFile, QString mapFile, QString out, QString maf, QString ms);
    bool transpose2binary(QString tpedFile, QString tfamFile, QString out, QString maf, QString ms);
    bool binary2transpose(QString binaryFile, QString out, QString maf, QString ms);
    bool runGWAS(QString phenotype, QString genotype, QString map,
                 QString covariate, QString kinship, QString model,
                 QString ms, QString maf, QString out);
    QStringList getParamList(void);    // Every param can't have any space.
    QString getParamString(void);
    QStringList getSupportedModel(void);

private:
    const QString toolpath;
    const QStringList model;
    QStringList paramlist;

    bool isVcfFile(QString file);
};

#endif // PLINK_H
