#include "plink.h"

Plink::Plink(): toolpath("/tool/plink"), model({"Linear", "Logistic"})
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

QStringList Plink::getSupportedModel(void)
{
    return this->model;
}

bool Plink::vcf2plink(QString vcfFile, QString out, QString maf, QString ms)
{
    if (vcfFile.isNull() || out.isNull())
    {
        return false;
    }

    //plink --vcf vcfFile [--maf maf] [--geno ms] --recode12 --out out

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

    //this->paramlist.append("--recode12");
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

    // plink --vcf vcfFile --maf maf --geno maf --recode12 --out out
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

bool Plink::vcf2transpose(QString vcfFile, QString out, QString maf, QString ms)
{
    if (vcfFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --vcf --maf maf --geno ms --recode12 --ouput-missing-genotype 0 --transpose --out out
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

bool Plink::plink2transpose(QString pedFile, QString mapFile, QString out, QString maf, QString ms)
{
    if (pedFile.isNull() || mapFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --ped pedFile --map mapFile [--maf maf] [--geno ms] --recode12 --transpose --out out
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

bool Plink::plink2binary(QString pedFile, QString mapFile, QString out, QString maf, QString ms)
{
    if (pedFile.isNull() || mapFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --ped .ped --map .map [--maf maf] [--geno ms] --make-bed --out out
    this->paramlist.clear();
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

    this->paramlist.append("--make-bed");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}

bool Plink::transpose2binary(QString tpedFile, QString tfamFile, QString out, QString maf, QString ms)
{
    if (tpedFile.isNull() || tfamFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --tped .tped --tfam .tfam [--maf maf] [--geno ms] --make-bed --out out
    this->paramlist.clear();
    this->paramlist.append("--tped");
    this->paramlist.append(tpedFile);
    this->paramlist.append("--tfam");
    this->paramlist.append(tfamFile);

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

bool Plink::binary2transpose(QString binaryFile, QString out, QString maf, QString ms)
{
    if (binaryFile.isNull() || out.isNull())
    {
        return false;
    }

    QFile bedFile(binaryFile+".bed");
    QFile bimFile(binaryFile+".bim");
    QFile famFile(binaryFile+".fam");

    if (!bedFile.exists() || !bimFile.exists() || !famFile.exists())
    {
        return false;
    }

    // plink --bfile binaryFile [--maf maf] [--geno ms] --recode12 --transpose --out out
    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("--bfile");
    this->paramlist.append(binaryFile);

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
    if (phenotype.isNull() || genotype.isNull() || model.isNull())
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

    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("--ped");
    this->paramlist.append(genotype);
    this->paramlist.append("--map");
    this->paramlist.append(map);
    //this->paramlist.append("--assoc");
    this->paramlist.append("--pheno");
    this->paramlist.append(phenotype);

    if (!covariate.isNull())
    {
        this->paramlist.append("--covar");
        this->paramlist.append(covariate);
    }
    if (!kinship.isNull())
    {   //

    }

    if (model == "Linear")
    {
        this->paramlist.append("--linear");
    }
    if (model == "Logistic")
    {
        this->paramlist.append("--logistic");
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

    this->paramlist.append("--allow-no-sex");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}
