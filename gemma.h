#ifndef GEMMA_H
#define GEMMA_H

#include <QMessageBox>
#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

#include <QDebug>
#include <QMessageBox>

class Gemma
{
public:
    Gemma();
    bool runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship, QString out, QString ms, QString maf, QString model);
    bool makeKinship(QString genotype, QString out);
    QStringList getParamList(void);    // Every param can't have any space.
    QString getParamString(void);
    QStringList getSupportedModel(void);
    bool phe_fam_Preparation(QString phe, QString fam);

private:
    const QString toolpath;
    const QStringList model;
    QStringList paramlist;

};

#endif // GEMMA_H
