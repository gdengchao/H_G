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
#include "tool.h"

class Plink : public Tool
{
public:
    Plink();
    bool vcf2transpose(QString vcfFile, QString out, QString maf, QString mind, QString geno);
    bool vcf2binary(QString vcfFile, QString out, QString maf, QString mind, QString geno);
    bool vcf2plink(QString vcfFile, QString out, QString maf, QString mind, QString geno);
    bool plink2transpose(QString pedFile, QString mapFile, QString out,
                         QString maf, QString mind, QString geno);
    bool plink2binary(QString pedFile, QString mapFile, QString out,
                      QString maf, QString mind, QString geno);
    bool transpose2binary(QString tpedFile, QString tfamFile, QString out,
                          QString maf, QString mind, QString geno);
    bool binary2transpose(QString binaryFile, QString out,
                          QString maf, QString mind, QString geno);
    bool runGWAS(QString phenotype, QString genotype, QString map, QString covariate,
                 QString kinship, QString model,QString out);
\
    void filterData(QString genotype, QString map, QString maf, QString mind, QString geno);

private:

    bool isVcfFile(QString file);
};

#endif // PLINK_H
