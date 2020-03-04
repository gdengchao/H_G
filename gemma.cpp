#include "gemma.h"

Gemma::Gemma(): toolpath("/tool/"), model({"mlm"})
{
    this->paramlist.clear();
}

QStringList Gemma::getParamList(void)
{
    return this->paramlist;
}

QString Gemma::getParamString(void)
{
    QString ret;
    for (QString item:this->paramlist)
    {
        ret += item + " ";
    }
    return ret;
}

bool Gemma:: makeKinship(QString genotype, QString out)
{
    if (genotype.isNull() || out.isNull())
    {
        return false;
    }

    // gemma -bfile 222_filter1 -gk 1 -o 222_filter1
    this->paramlist.clear();
    this->paramlist.append("-bfile");
    this->paramlist.append(genotype);
    this->paramlist.append("-gk");
    this->paramlist.append("1");
    this->paramlist.append("-o");
    this->paramlist.append(out);

    return true;
}

// genotype must be the prefix of binary file.
bool Gemma::runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship, QString out)
{
    this->paramlist.clear();            // Clear paramlist before set parameter.
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

    // gemma -bfile 222_filter1 -k 222_filter1.cXX.txt -lmm 1 -n 2 -o 222_filter1
    this->paramlist.append("-bfile");
    this->paramlist.append(genotype);
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
