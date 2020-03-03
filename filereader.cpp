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
