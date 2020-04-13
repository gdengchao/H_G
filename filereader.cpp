#include "filereader.h"

FileReader::FileReader()
{

}

void FileReader::setPhenotypeFile(QString const dir)
{
    if (!dir.isEmpty())
    {
        this->phenotypeFile = dir;
    }
}

QString FileReader::getPhenotypeFile(void)
{
    return this->phenotypeFile;
}

void FileReader::setGenotypeFile(QString const dir)
{
    if (!dir.isEmpty())
    {
        this->genotypeFile = dir;
    }
}

QString FileReader::getGenotypeFile(void)
{
    return this->genotypeFile;
}

void FileReader::setMapFile(QString const dir)
{
    if (!dir.isEmpty())
    {
        this->mapFile = dir;
    }
}

QString FileReader::getMapFile(void)
{
    return this->mapFile;
}

void FileReader::setCovariateFile(QString const dir)
{
    if (!dir.isEmpty())
    {
        this->covariateFile = dir;
    }
}

QString FileReader::getCovariateFile(void)
{
    return this->covariateFile;
}

void FileReader::setKinshipFile(QString const dir)
{
    if (!dir.isEmpty())
    {
        this->kinshipFile = dir;
    }
}

QString FileReader::getKinshipFile(void)
{
    return this->kinshipFile;
}

/**
 * @brief FileReader::getFIDList
 * @param src
 * @param col: the No.col column is the fid in src file.
 * @return
 */
QStringList FileReader::getFIDList(QString const src, int col)
{
    QStringList fidList;
    if (src.isNull() || col < 1)
    {
        return fidList;
    }
    QFile file(src);
    QTextStream fileStream(&file);
    if (file.open(QIODevice::ReadOnly))
    {
        return fidList;
    }
    while (!fileStream.atEnd())
    {
        QStringList curLine = fileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (col > curLine.length())
        {
            return fidList;
        }
        if (fidList.indexOf(curLine[col-1]) == -1)
        {
            fidList.append(curLine[col-1]);
        }
    }
    return fidList;
}
