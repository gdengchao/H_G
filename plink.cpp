#include "plink.h"

Plink::Plink()
{
    this->model << "Linear" << "Logistic";
    this->paramlist.clear();
}

bool Plink::vcf2plink(QString vcfFile, QString out, QString maf, QString mind, QString geno)
{
    if (vcfFile.isNull() || out.isNull())
    {
        return false;
    }

    //plink --vcf vcfFile [--maf maf] [--mind mind] [--geno geno] --recode --out out

    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("--vcf");
    this->paramlist.append(vcfFile);

    if (!maf.isNull())
    {
        this->paramlist.append("--maf");
        this->paramlist.append(maf);
    }
    if (!mind.isNull())
    {
        this->paramlist.append("--mind");
        this->paramlist.append(mind);
    }
    if (!geno.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(geno);
    }

    this->paramlist.append("--recode");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}


bool Plink::vcf2binary(QString vcfFile, QString out, QString maf, QString mind, QString geno)
{
    if (vcfFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --vcf vcfFile [--maf maf] [--mind mind] [--geno geno] --make-bed --out out
    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("--vcf");
    this->paramlist.append(vcfFile);

    if (!maf.isNull())
    {
        this->paramlist.append("--maf");
        this->paramlist.append(maf);
    }
    if (!mind.isNull())
    {
        this->paramlist.append("--mind");
        this->paramlist.append(mind);
    }
    if (!geno.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(geno);
    }

    this->paramlist.append("--make-bed");
    this->paramlist.append("--out");
    this->paramlist.append(out);

    return true;
}

bool Plink::vcf2transpose(QString vcfFile, QString out, QString maf, QString mind, QString geno)
{
    if (vcfFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --vcf [--maf maf] [--mind mind] [--geno geno] --recode12 --ouput-missing-genotype 0 --transpose --out out
    this->paramlist.clear();            // Clear paramlist before set parameter.
    this->paramlist.append("--vcf");
    this->paramlist.append(vcfFile);

    if (!maf.isNull())
    {
        this->paramlist.append("--maf");
        this->paramlist.append(maf);
    }
    if (!mind.isNull())
    {
        this->paramlist.append("--mind");
        this->paramlist.append(mind);
    }
    if (!geno.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(geno);
    }

    this->paramlist.append("--recode12");
    this->paramlist.append("--output-missing-genotype");
    this->paramlist.append("0");
    this->paramlist.append("--transpose");
    this->paramlist.append("--out");
    this->paramlist.append(out);

    return true;
}

bool Plink::plink2transpose(QString pedFile, QString mapFile, QString out, QString maf, QString mind, QString geno)
{
    if (pedFile.isNull() || mapFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --ped pedFile --map mapFile [--maf maf] [--mind mind] [--geno geno] --recode12 --transpose --out out
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
    if (!mind.isNull())
    {
        this->paramlist.append("--mind");
        this->paramlist.append(mind);
    }

    if (!geno.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(geno);
    }

    this->paramlist.append("--recode12");
    this->paramlist.append("--output-missing-genotype");
    this->paramlist.append("0");
    this->paramlist.append("--transpose");
    this->paramlist.append("--out");
    this->paramlist.append(out);

    return true;
}

bool Plink::plink2binary(QString pedFile, QString mapFile, QString out, QString maf, QString mind, QString geno)
{
    if (pedFile.isNull() || mapFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --ped .ped --map .map [--maf maf] [--mind mind] [--geno geno] --make-bed --out out
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
    if (!mind.isNull())
    {
        this->paramlist.append("--mind");
        this->paramlist.append(mind);
    }
    if (!geno.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(geno);
    }

    this->paramlist.append("--make-bed");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}

bool Plink::transpose2binary(QString tpedFile, QString tfamFile, QString out, QString maf, QString mind, QString geno)
{
    if (tpedFile.isNull() || tfamFile.isNull() || out.isNull())
    {
        return false;
    }

    // plink --tped .tped --tfam .tfam [--maf maf] [--mind mind] [--geno geno] --make-bed --out out
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
    if (!mind.isNull())
    {
        this->paramlist.append("--mind");
        this->paramlist.append(mind);
    }
    if (!geno.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(geno);
    }

    this->paramlist.append("--make-bed");
    this->paramlist.append("--out");
    this->paramlist.append(out);
    //this->paramlist.append("--noweb");

    return true;
}

bool Plink::binary2transpose(QString binaryFile, QString out, QString maf, QString mind, QString geno)
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
    if (!mind.isNull())
    {
        this->paramlist.append("--mind");
        this->paramlist.append(mind);
    }
    if (!geno.isNull())
    {
        this->paramlist.append("--geno");
        this->paramlist.append(geno);
    }

    this->paramlist.append("--recode12");
    this->paramlist.append("--output-missing-genotype");
    this->paramlist.append("0");
    this->paramlist.append("--transpose");
    this->paramlist.append("--out");
    this->paramlist.append(out);

    return true;
}

bool Plink::runGWAS(QString phenotype, QString genotype, QString map, QString covariate,
                    QString kinship, QString model, QString out)
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

    // Genotype file info.
    QFileInfo genoFileInfo = QFileInfo(genotype);
    QString genoFileName = genoFileInfo.fileName();         // demo.vcf.gz
    QString genoFileBaseName = genoFileInfo.baseName();     // geno
    QString genoFileSuffix = genoFileInfo.suffix();         //
    QString genoFileAbPath = genoFileInfo.absolutePath();

    this->paramlist.clear();            // Clear paramlist before set parameter.
    if (isVcfFile(genotype)) // Transform "vcf" to "transpose"
    {
        this->paramlist.append("--vcf");
        this->paramlist.append(genotype);
    }
    if (genotype.split(".")[genotype.split(".").length()-1] == "ped")
    {
        if (map.isNull())
        {
            map = genoFileAbPath+"/"+genoFileBaseName+".map";
        }
        this->paramlist.append("--ped");
        this->paramlist.append(genotype);
        this->paramlist.append("--map");
        this->paramlist.append(map);
    }

    if (genotype.split(".")[genotype.split(".").length()-1] == "tped")
    {
        if (map.isNull())
        {
            map = genoFileAbPath+"/"+genoFileBaseName+".tfam";
        }
        this->paramlist.append("--tped");
        this->paramlist.append(genotype);
        this->paramlist.append("--tfam");
        this->paramlist.append(map);
    }

    if (genotype.split(".")[genotype.split(".").length()-1] == "bed")
    {
        this->paramlist.append("--bfile");
        this->paramlist.append(genoFileAbPath+"/"+genoFileBaseName);
    }

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

    this->paramlist.append("--allow-no-sex");
    this->paramlist.append("--out");
    this->paramlist.append(out);

    return true;
}

void Plink::filterData(QString genotype, QString map, QString maf, QString mind, QString geno)
{
    if (genotype.isNull() || map.isNull())
    {
        return;
    }

    QFileInfo genoFileInfo(geno);
    QString genoFileBaseName = genoFileInfo.baseName();
    QString genoFileAbPath = genoFileInfo.absolutePath();
    QString genoFileSuffix = genoFileInfo.suffix();

    if (this->isVcfFile(genotype))
    {

    }

    if (genoFileSuffix == "ped")
    {

    }

    if (genoFileSuffix == "tped")
    {

    }

    if (genoFileSuffix == "bed")
    {

    }
}

bool Plink::isVcfFile(QString file) // Just consider filename.
{
    if (file.isNull() || file.isEmpty())
    {
        return false;
    }

    QFileInfo fileInfo = QFileInfo(file);
    QStringList fileNameList = fileInfo.fileName().split(".");         // demo.vcf.gz

    for (QString item : fileNameList)
    {
        if (item == "vcf")
        {
            return true;
        }
    }

    return false;
}
