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


bool PopLDdecay::runLD(QString ped, QString map, QString out)
{
    if (ped.isNull() || map.isNull() || out.isNull())
    {
        return false;
    }
    // PopLDdecay
    this->paramlist.clear();


    return true;
}
