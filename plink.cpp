#include "plink.h"

Plink::Plink(): toolpath("/tool/plink/"), model({"glm", "mlm", "linear_regression"})
{
    this->paramlist.clear();
}

QStringList Plink::getParamList(void)
{
    return this->paramlist;
}

QString Plink::getParamString(void)
{
    QString ret;
    for (QString item:this->paramlist)
    {
        ret += item + " ";
    }
    return ret;
}

/*
 * FileType: vcf plink binary trsanform
 *
 */
bool Plink::transformFile(QString srcFileType, QString dstFileType)
{
    if (srcFileType == "plink" && dstFileType == "transform")
    {

    }
    return true;
}

bool Plink::runGWAS(QString phenotype, QString genotype, QString map,
                    QString covariate, QString kinship, QString model,
                    QString ms, QString maf, QString out)
{
    if (!phenotype.isNull() && !genotype.isNull() && !map.isNull())
    {
        if (genotype.split('.')[genotype.split('.').length()-1] == "ped")
        {
            this->paramlist.append("--ped");
            this->paramlist.append(genotype);
            this->paramlist.append("--map");
            this->paramlist.append(map);
            this->paramlist.append("--assoc");
            this->paramlist.append("--pheno");
            this->paramlist.append(phenotype);
        }
        if (!covariate.isNull())
        {
            this->paramlist.append("--covar");
            this->paramlist.append(covariate);
        }
        if (!kinship.isNull())
        {   //

        }
        if (model.isNull())
        {
            if (model == "glm")
            {
                this->paramlist.append("--linear");
            }
            if (model == "logistic regression")
            {
                this->paramlist.append("--logistic");
            }
        }
        if (!ms.isNull())
        {
            this->paramlist.append("--geno");
            this->paramlist.append(ms);
        }
        if (!maf.isNull())
        {
            this->paramlist.append("--maf");
            this->paramlist.append(maf);
        }
        this->paramlist.append("--out");
        this->paramlist.append(out);
        this->paramlist.append("--noweb");
    }
    else
    {
        if (phenotype.isNull())
        {
            QMessageBox::information(nullptr, "Error", "A Phenotype File is Needed!    ",   QMessageBox::Yes, QMessageBox::Yes);
        }
        if (genotype.isNull())
        {
            QMessageBox::information(nullptr, "Error", "A Genotype File is Needed!    ",   QMessageBox::Yes, QMessageBox::Yes);
        }
        return false;
    }
    return true;
}

