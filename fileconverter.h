#ifndef FILECONVERTER_H
#define FILECONVERTER_H

#include <QString>

class FileConverter
{
public:
    FileConverter();
    bool converFile(QString srcType, QString srcFile, QString dstType, QString dstFile);
    bool vcf2plink(QString srcFile, QString dstFile);
};

#endif // FILECONVERTER_H
