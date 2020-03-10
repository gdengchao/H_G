#ifndef EMMAX_H
#define EMMAX_H

#include <QString>
#include <QMessageBox>

class Emmax
{
public:
    Emmax();

    bool runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship, QString out);
    bool makeKinship(QString genotype);
    QStringList getParamList(void);    // Every param can't have any space.
    QString getParamString(void);
    QStringList getSupportedModel(void);
private:
    const QString toolpath;
    const QStringList model;
    QStringList paramlist;
};

#endif // EMMAX_H
