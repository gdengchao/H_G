#include "fileconverter.h"

FileConverter::FileConverter()
{

}

// file type: vcf plink trsanform binary
// dstFile won't include the postfix.
bool FileConverter::converFile(QString srcType, QString srcFile, QString dstType, QString dstFile)
{
    if (srcType == "vcf" && dstType == "plink")
    {
        return this->vcf2plink(srcFile, dstFile) ? true : false;
    }

    return true;
}

bool FileConverter::vcf2plink(QString srcFile, QString dstFile)
{
    if (srcFile.isNull() || dstFile.isNull())
    {
        return false;
    }

    return true;
}

