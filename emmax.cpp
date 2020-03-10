#include "emmax.h"

Emmax::Emmax(): toolpath("/tool/emmax"), model({"EMMA"})
{
    this->paramlist.clear();
}

QStringList Emmax::getParamList(void)
{
    return this->paramlist;
}

QString Emmax::getParamString(void)
{
    QString ret;
    for (QString item:this->paramlist)
    {
        ret += item + " ";
    }
    return ret;
}

QStringList Emmax::getSupportedModel(void)
{
    return this->model;
}

bool Emmax:: makeKinship(QString genotype)
{
    if (genotype.isNull())
    {
        return false;
    }

    // emmax-kin -v -h -d 10 222_filter1_rec12
    this->paramlist.clear();
    this->paramlist.append("-v");
    this->paramlist.append("-h");
    this->paramlist.append("-d");
    this->paramlist.append("10");
    this->paramlist.append(genotype);

    return true;
}

// genotype must be the prefix of tped/fam file.
bool Emmax::runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship, QString out)
{
    if (genotype.isNull() || phenotype.isNull())
    {
        if (phenotype.isNull())
        {
            QMessageBox::information(nullptr, "Error", "A Phenotype File is Needed!    ",   QMessageBox::Yes, QMessageBox::Yes);
        }
        if (genotype.isNull())
        {
            QMessageBox::information(nullptr, "Error", "A Transpose File is Needed!    ",   QMessageBox::Yes, QMessageBox::Yes);
        }
        return false;
    }
    // emmax -v -d 10 -t 222_filter1_rec12 -p phe1_fall_time -k 222_filter1_rec12.hBN.kinf -o 222_filter1_phe1_BN
    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("-v");
    this->paramlist.append("-d");
    this->paramlist.append("10");
    this->paramlist.append("-t");
    this->paramlist.append(genotype);
    this->paramlist.append("-p");
    this->paramlist.append(phenotype);

    if (!covariate.isNull())
    {
        this->paramlist.append("-c");
        this->paramlist.append(covariate);
    }
    if (!kinship.isNull())
    {   //
        this->paramlist.append("-k");
        this->paramlist.append(kinship);
    }

    this->paramlist.append("-o");
    this->paramlist.append(out);

    return true;
}

