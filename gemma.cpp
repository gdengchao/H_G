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
    this->paramlist.append("-p");
    this->paramlist.append(phenotype);
    this->paramlist.append("-k");
    this->paramlist.append(kinship);
    this->paramlist.append("-lmm");
    this->paramlist.append("1");
    this->paramlist.append("-n");
    this->paramlist.append("2");

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

    tmpFamFile.open(QIODevice::ReadWrite);

    QFileInfo famFileInfo(fam), tmpFamFileInfo(tmpFamFile);
    QString famFileName = famFileInfo.fileName();
    QString famFileAbPath = famFileInfo.absolutePath();
    QString tmpFamFileAbFilePaht = tmpFamFileInfo.absoluteFilePath();

    QTextStream tmpFamStream(&tmpFamFile);
    QTextStream pheStream(&pheFile);
    QTextStream famStream(&famFile);

    if (!pheFile.open(QIODevice::ReadOnly) || !famFile.open(QIODevice::ReadOnly))
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
        famCurLineList.removeLast();
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
    tmpFamFile.rename(tmpFamFileAbFilePaht, famFileAbPath+"/"+famFileName);

    return true;
}
