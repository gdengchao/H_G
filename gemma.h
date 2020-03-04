#ifndef GEMMA_H
#define GEMMA_H

#include <QMessageBox>
#include <QStringList>

class Gemma
{
public:
    Gemma();
    bool runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship, QString out);
    bool makeKinship(QString genotype, QString out);
    QStringList getParamList(void);    // Every param can't have any space.
    QString getParamString(void);

private:
    const QString toolpath;
    const QStringList model;
    QStringList paramlist;
};

#endif // GEMMA_H
