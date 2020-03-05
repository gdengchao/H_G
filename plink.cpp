#include "plink.h"

Plink::Plink(): toolpath("/tool/plink"), model({"glm", "mlm", "linear_regression"})
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

bool Plink::vcf2transpose(QString vcfFile, QString out, QString maf, QString ms)
{
    if (vcfFile.isNull() || out.isNull())
    {
        return false;
    }

    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("--vcf");
    this->paramlist.append(vcfFile);

    if (!maf.isNull())
    {
        this->paramlist.append("--maf");
        this->paramlist.append(maf);
    }
    if (!ms.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(ms);
    }

    this->paramlist.append("--recode12");
    this->paramlist.append("--output-missing-genotype");
    this->paramlist.append("0");
    this->paramlist.append("--transpose");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}

bool Plink::vcf2binary(QString vcfFile, QString out, QString maf, QString ms)
{
    if (vcfFile.isNull() || out.isNull())
    {
        return false;
    }

    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("--vcf");
    this->paramlist.append(vcfFile);

    if (!maf.isNull())
    {
        this->paramlist.append("--maf");
        this->paramlist.append(maf);
    }
    if (!ms.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(ms);
    }

    this->paramlist.append("--make-bed");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}

bool Plink::plink2transpose(QString pedFile, QString mapFile, QString out, QString maf, QString ms)
{
    if (pedFile.isNull() || mapFile.isNull() || out.isNull())
    {
        return false;
    }

    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("--ped");
    this->paramlist.append(pedFile);
    this->paramlist.append("--map");
    this->paramlist.append(mapFile);

    if (!maf.isNull())
    {
        this->paramlist.append("--maf");
        this->paramlist.append(maf);
    }
    if (!ms.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(ms);
    }

    this->paramlist.append("--recode12");
    this->paramlist.append("--output-missing-genotype");
    this->paramlist.append("0");
    this->paramlist.append("--transpose");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}

bool Plink::runGWAS(QString phenotype, QString genotype, QString map,
                    QString covariate, QString kinship, QString model,
                    QString ms, QString maf, QString out)
{
    this->paramlist.clear();            // Clear paramlist before set parameter.
    if (phenotype.isNull() || genotype.isNull())
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
        this->paramlist.append("--mind");
        this->paramlist.append(ms);
    }
    if (!maf.isNull())
    {
        this->paramlist.append("--maf");
        this->paramlist.append(maf);
    }
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}

bool Plink::plink2binary(QString pedFile, QString mapFile, QString out)
{
    if (pedFile.isNull() || mapFile.isNull() || out.isNull())
    {
        return false;
    }

    this->paramlist.clear();
    this->paramlist.append("--ped");
    this->paramlist.append(pedFile);
    this->paramlist.append("--map");
    this->paramlist.append(mapFile);
    this->paramlist.append("--make-bed");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}

