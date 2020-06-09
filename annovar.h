#ifndef ANNOVAR_H
#define ANNOVAR_H

#include <QFile>
#include "tool.h"

class Annovar : public Tool
{
public:
    Annovar();
    bool gffTogtf(QString gffFile, QString outFile);
    bool gtfToGenePred(QString gtfFile, QString outFile);
    bool retrieveSeqFromFasta(QString refGeneFile, QString seqFile, QString outFile);
    bool tableAnnovar(QString avinput, QString refGeneDir, QString refGenePrefix, QString out);
    bool annotateVariation(QString avinput, QString refGeneDir, QString refGenePrefix, QString out);
};

#endif // ANNOVAR_H
