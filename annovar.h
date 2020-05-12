#ifndef ANNOVAR_H
#define ANNOVAR_H

#include "tool.h"

class Annovar : public Tool
{
public:
    Annovar();
    bool gffTogtf(QString gffFile, QString outFile);
    bool gtfToGenePred(QString gtfFile, QString outFile);
    bool retrieveSeqFromFasta(QString refGeneFile, QString seqFile, QString outFile);
};

#endif // ANNOVAR_H
