#include "emmax.h"

Emmax::Emmax(): toolpath("/tool/emmax"), model({"mlm"})
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


// genotype must be the prefix of tped/fam file.
bool Emmax::runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship, QString out)
{
    this->paramlist.clear();            // Clear paramlist before set parameter.
    if (!genotype.isNull() && !phenotype.isNull() && !phenotype.isNull())
    {
        // emmax -v -d 10 -t 222_filter1_rec12 -p phe1_fall_time -k 222_filter1_rec12.hBN.kinf -o 222_filter1_phe1_BN
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
    }
    else
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
    return true;
}

