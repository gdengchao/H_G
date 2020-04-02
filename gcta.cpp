#include "gcta.h"

Gcta::Gcta() : toolpath("/Gcta/")
{

}

QStringList Gcta::getParamList(void)
{
    return this->paramlist;
}

QString Gcta::getParamString(void)
{
    QString ret;
    for (QString item:this->paramlist)
    {
        ret += item + " ";
    }
    return ret;
}

void Gcta::setParam(QStringList param)
{
    this->paramlist = param;
}

void Gcta::setParam(QString param)
{
    this->paramlist = param.split(QRegExp("\\s+"), QString::SkipEmptyParts);
}


QString Gcta::getToolPath()
{
    return this->toolpath;
}

bool Gcta::makeGRM(QString binGenoBaseName, QString out)
{
    if (binGenoBaseName.isNull() || out.isNull())
    {
        return false;
    }
    this->paramlist.clear();

    // gcta64 --bfile 222_filter1_rec12 --make-grm --out 222_filter1_rec12
    this->paramlist.append("--bfile");
    this->paramlist.append(binGenoBaseName);
    this->paramlist.append("--make-grm");
    this->paramlist.append("--out");
    this->paramlist.append(out);

    return true;
}

bool Gcta::runPCA(QString grmBaseName, int PCs, int threads, QString out)
{
    if (grmBaseName.isNull() || out.isNull() || PCs < 1 || threads < 1)
    {
        return false;
    }
    // gcta64 --grm 222_filter1_rec12 --pca 4 --out 222_filter1_rec12 --threads 56;
    this->paramlist.clear();

    this->paramlist.append("--grm");
    this->paramlist.append(grmBaseName);
    this->paramlist.append("--pca");
    this->paramlist.append(QString::number(PCs));
    this->paramlist.append("--out");
    this->paramlist.append(out);
    this->paramlist.append("--threads");
    this->paramlist.append(QString::number(threads));

    return true;
}
