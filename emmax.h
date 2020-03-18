#ifndef EMMAX_H
#define EMMAX_H

#include <QString>
#include <QMessageBox>
#include "tool.h"

class Emmax : public Tool
{
public:
    Emmax();

    bool runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship, QString out);
    bool makeKinship(QString genotype);
};

#endif // EMMAX_H
