#ifndef FILEREADER_H
#define FILEREADER_H

#include <QString>
#include "enum.h"
#include <QDebug>

class FileReader
{
public:
    FileReader();
    void setPhenotypeFile(QString const dir);
    QString getPhenotypeFile(void);
    void setGenotypeFile(QString const dir);
    QString getGenotypeFile(void);
    void setMapFile(QString dir);
    QString getMapFile(void);
    void setCovariateFile(QString const dir);
    QString getCovariateFile(void);
    void setKinshipFile(QString const dir);
    QString getKinshipFile(void);

private:
    QString phenotypeFile;
    QString genotypeFile;
    QString mapFile;
    QString covariateFile;
    QString kinshipFile;
};

#endif // FILEREADER_H
