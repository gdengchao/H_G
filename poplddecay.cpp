#include "poplddecay.h"

PopLDdecay::PopLDdecay() : toolpath("/tools/")
{

}


QStringList PopLDdecay::getParamList(void)
{
    return this->paramlist;
}

QString PopLDdecay::getParamString(void)
{
    QString ret;
    for (QString item:this->paramlist)
    {
        ret += item + " ";
    }
    return ret;
}

void PopLDdecay::setParam(QStringList param)
{
    this->paramlist = param;
}

void PopLDdecay::setParam(QString param)
{
    this->paramlist = param.split(QRegExp("\\s+"), QString::SkipEmptyParts);
}


QString PopLDdecay::getToolPath()
{
    return this->toolpath;
}

bool PopLDdecay::preGenotype(QString ped, QString map, QString out)
{
    if (ped.isNull() || map.isNull() || out.isNull())
    {
        return false;
    }
    /*
     * perl ~/software/PopLDdecay-master/bin/mis/plink2genotype.pl -inPED 222_filter1.ped
     *  -inMAP 222_filter1.map -outGenotype 222_filter1.genotype
     */
    this->paramlist.clear();
    this->paramlist.append("-inPED");
    this->paramlist.append(ped);
    this->paramlist.append("-inMAP");
    this->paramlist.append(map);
    this->paramlist.append("-outGenotype");
    this->paramlist.append(out);

    return true;
}

bool PopLDdecay::runLD(QString _genotype, QString out)
{
    if (_genotype.isNull() || out.isNull())
    {
        return false;
    }
     // PopLDdecay -InGenotype 222_filter1.genotype -OutStat 222_filter1_LD
    this->paramlist.clear();

    this->paramlist.append("-InGenotype");
    this->paramlist.append(_genotype);
    this->paramlist.append("-OutStat");
    this->paramlist.append(out);

    return true;
}



bool PopLDdecay::plotLD(QString in, QString out)
{
    if (in.isNull() || out.isNull())
    {
        return false;
    }

    /*
     * perl ~/software/PopLDdecay-master/bin/Plot_OnePop.pl
     * -inFile 222_filter1_LD.stat.gz -output 222_filter1_LD_fig
     */
    this->paramlist.clear();
    this->paramlist.append("-inFile");
    this->paramlist.append(in);
    this->paramlist.append("-output");
    this->paramlist.append(out);

    return true;
}

QStringList PopLDdecay::makeKeepFromTranspose(QString tfam)
{
    QStringList keepFileList;
    QStringList fidList;
    if (tfam.isNull())
    {
        return keepFileList;
    }

    QFile keepFile;
    QTextStream keepFileStream(&keepFile);
    QFile tfile(tfam);
    QFileInfo tfileInfo(tfile);
    QString tfileBaseName = tfileInfo.baseName();
    QString tfileAbPath = tfileInfo.absolutePath();

    QTextStream tfileStream(&tfile);

    if (!tfile.open(QIODevice::ReadOnly))
    {
        return keepFileList;
    }

    while (!tfileStream.atEnd())
    {
        QStringList curLine = tfileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (fidList.indexOf(curLine[0]) == -1)
        {   // The first appearence of the FID.
            keepFile.setFileName(tfileAbPath+"/"+tfileBaseName+"_"+curLine[0]+".keep");
            keepFile.open(QIODevice::Append);
            keepFileList.append(tfileAbPath+"/"+tfileBaseName+"_"+curLine[0]+".keep");
            fidList.append(curLine[0]);
        }
        keepFileStream << curLine[0] << "\t" << curLine[1] << "\n";
    }

    for (auto item:fidList)
    {
        keepFile.setFileName(tfileAbPath+"/"+tfileBaseName+"_"+item+".keep");
        keepFile.close();
    }

    return keepFileList;
}
