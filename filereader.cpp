#include "filereader.h"

FileReader::FileReader()
{

}

QString FileReader::lastDirectory = "Hello";

void FileReader::setPhenotypeFile(QString dir)
{
    this->phenotypeFile = dir;
}

QString FileReader::getPhenotypeFile(void)
{
    return this->phenotypeFile;
}

void FileReader::setGenotypeFile(QString dir)
{
    this->genotypeFile = dir;
}

QString FileReader::getGenotypeFile(void)
{
    return this->genotypeFile;
}

void FileReader::setMapFile(QString dir)
{
    this->mapFile = dir;
}

QString FileReader::getMapFile(void)
{
    return this->mapFile;
}

void FileReader::setCovariateFile(QString dir)
{
    this->covariateFile = dir;
}

QString FileReader::getCovariateFile(void)
{
    return this->covariateFile;
}

void FileReader::setKinshipFile(QString dir)
{
    this->kinshipFile = dir;
}

QString FileReader::getKinshipFile(void)
{
    return this->kinshipFile;
}
