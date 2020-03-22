#include "gemma.h"

Gemma::Gemma()
{
    this->model << "LMM" << "BSLMM";
    this->paramlist.clear();
}

/**
 * @brief Gemma::makeKinship
 * @param genotype:binary file, without suffix, need .bed/.fam/.bim in the same path.
 * @param out: a file base name.
 * @return
 */
bool Gemma:: makeKinship(QString genotype, QString out, QMap<QString, QString> moreParam)
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

/**
 * @brief Gemma::runGWAS
 * @param genotype: the prefix of binary file.
 * @param phenotype
 * @param covariate
 * @param kinship
 * @param out
 * @param model
 * @return
 */
bool Gemma::runGWAS(QString genotype, QString phenotype, QString covariate, QString kinship,
                    QString out, QString model, QMap<QString, QString> moreParam)
{
    this->paramlist.clear();            // Clear paramlist before set parameter.
    if (genotype.isNull() || phenotype.isNull() || kinship.isNull() || model.isNull())
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

    this->paramlist.append("-bfile");
    this->paramlist.append(genotype);

    if (model == "LMM")
    {   // gemma -bfile 222_filter1 -k 222_filter1.cXX.txt -lmm 1 -n 2 -o 222_filter1
        this->paramlist.append("-k");
        this->paramlist.append(kinship);
        this->paramlist.append("-lmm");
        this->paramlist.append(moreParam["lmmtest"]);   // wald:1   likelihood:2    score:3     all:4
        this->paramlist.append("-n");
        this->paramlist.append("2");
    }

    if (model == "BSLMM")
    {  // gemma -bfile 222_filter1 -bslmm 1 -n 2 -o tmp
        this->paramlist.append("-bslmm");
        this->paramlist.append("1");
        this->paramlist.append("-n");
        this->paramlist.append("2");
    }

    if (!covariate.isNull())
    {
        this->paramlist.append("-c");
        this->paramlist.append(covariate);
    }

    this->paramlist.append("-o");
    this->paramlist.append(out);

    return true;
}

bool Gemma::phe_fam_Preparation(QString phe, QString fam)
{   // Replace "NA" to "-9", then complete .fam
    // .fam: FID IID PID MID Sex 1 Phe  (phenotype data to the 7th column of .fam)
    if (phe.isNull() || fam.isNull())
    {
        return false;
    }

    QFile pheFile(phe);
    QFile famFile(fam);
    QFile tmpFamFile(fam+".tmp");  // A temp file to save the final data of fam file.


    QFileInfo famFileInfo(fam), tmpFamFileInfo(tmpFamFile);
    QString famFileName = famFileInfo.fileName();
    QString famFileAbPath = famFileInfo.absolutePath();
    QString tmpFamFileAbFilePath = tmpFamFileInfo.absoluteFilePath();   // /path/basename.suffix

    QTextStream tmpFamStream(&tmpFamFile);
    QTextStream pheStream(&pheFile);
    QTextStream famStream(&famFile);

    if (!pheFile.open(QIODevice::ReadOnly) ||
        !famFile.open(QIODevice::ReadOnly) ||
        !tmpFamFile.open(QIODevice::ReadWrite))
    {
        return false;
    }
    while (!pheStream.atEnd() && !famStream.atEnd())
    {
        QString famCurLine = famStream.readLine();
        QString pheCurLine = pheStream.readLine();
        // Replace "NA" to "-9"
        QStringList pheCurLineList = pheCurLine.replace("NA", "-9").split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QStringList famCurLineList = famCurLine.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        while (famCurLineList.length() > 5)
        {
            famCurLineList.removeLast();
        }

        // .fam: FID IID PID MID Sex 1 Phe  (phenotype data to the 7th column of .fam)
        tmpFamStream << famCurLineList.join("\t") << "\t1\t" << pheCurLineList[pheCurLineList.length()-1] << "\n";
    }

    if (!pheStream.atEnd() || !famStream.atEnd())
    {
        return false;
    }
    pheFile.close();
    famFile.close();
    tmpFamFile.close();

    famFile.remove();
    tmpFamFile.rename(tmpFamFileAbFilePath, famFileAbPath+"/"+famFileName);

    return true;
}
