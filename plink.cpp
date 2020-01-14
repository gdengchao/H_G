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
                    QString out, QString ms, QString maf)
{
    if (!phenotype.isNull() && !genotype.isNull() && !map.isNull())
    {
        if (genotype.split('.')[genotype.split('.').length()-1] == "ped")
        {
            this->paramlist.append("--ped " + genotype);
            this->paramlist.append("--map " + map);
            this->paramlist.append("--pheno " + phenotype);
            //this->paramlist.append("--maf " + maf);
            this->paramlist.append("--out " + out);
            this->paramlist.append("--assco");
            this->paramlist.append("--noweb");
        }
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

