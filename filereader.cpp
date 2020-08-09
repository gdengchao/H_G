#include "filereader.h"

FileReader::FileReader()
{

}

void FileReader::setPhenotypeFile(QString const dir)
{
    this->phenotypeFile = dir;
}

QString FileReader::getPhenotypeFile(void)
{
    return this->phenotypeFile;
}

void FileReader::setGenotypeFile(QString const dir)
{
    this->genotypeFile = dir;
}

QString FileReader::getGenotypeFile(void)
{
    return this->genotypeFile;
}

void FileReader::setMapFile(QString const dir)
{
    this->mapFile = dir;
}

QString FileReader::getMapFile(void)
{
    return this->mapFile;
}

void FileReader::setCovariateFile(QString const dir)
{
    this->covariateFile = dir;
}

QString FileReader::getCovariateFile(void)
{
    return this->covariateFile;
}

void FileReader::setKinshipFile(QString const dir)
{
    this->kinshipFile = dir;
}

QString FileReader::getKinshipFile(void)
{
    return this->kinshipFile;
}

/**
 * @brief FileReader::getFIDList
 *          get family ID in src file.
 * @param src
 * @param col: the No.col column is the fid in src file.
 * @return
 */
QStringList FileReader::getFIDList(QString const src, int col)
{
    QStringList fidList;
    if (src.isNull() || col < 1)
    {   // The path is not correct or the objective column is wrong.
        return fidList; // return empty list.
    }
    QFile file(src);
    QTextStream fileStream(&file);
    if (!file.open(QIODevice::ReadOnly))
    {   // Open file error.
        return fidList;
    }
    while (!fileStream.atEnd())
    {   // Read a line and split it by space(s).
        QStringList curLine = fileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (col > curLine.length())
        {   // The objective column bigger than length of current line.
            return fidList; // return empty list.
        }
        if (fidList.indexOf(curLine[col-1]) == -1)
        {   // Append to FID list if not in FID list.
            fidList.append(curLine[col-1]);
        }
        qApp->processEvents();  // Prevent "no responding" of MainWindow.
    }
    return fidList;
}
