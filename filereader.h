#ifndef FILEREADER_H
#define FILEREADER_H

#include <QString>
#include "enum.h"
#include <QDebug>

class FileReader
{
public:
    FileReader();
    void setPhenotypeFile(QString dir);
    QString getPhenotypeFile(void);
    void setGenotypeFile(QString dir);
    QString getGenotypeFile(void);
    void setMapFile(QString dir);
    QString getMapFile(void);
    void setCovariateFile(QString dir);
    QString getCovariateFile(void);
    void setKinshipFile(QString dir);
    QString getKinshipFile(void);

    static QString lastDirectory;
private:
    QString phenotypeFile;
    QString genotypeFile;
    QString mapFile;
    QString covariateFile;
    QString kinshipFile;
};

#endif // FILEREADER_H
