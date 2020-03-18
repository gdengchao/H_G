#ifndef GEMMA_H
#define GEMMA_H

#include <QMessageBox>
#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <tool.h>

class Gemma : public Tool
{
public:
    Gemma();
    bool runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship, QString out, QString model);
    bool makeKinship(QString genotype, QString out);
    bool phe_fam_Preparation(QString phe, QString fam);
};

#endif // GEMMA_H
