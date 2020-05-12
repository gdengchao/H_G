#include "annovar.h"

Annovar::Annovar()
{

}

bool Annovar::gffTogtf(QString gffFile, QString outFile)
{
    if (gffFile.isNull() || outFile.isNull())
    {
        return false;
    }

    //  gffread Hdhv3.changeID.gff3 -T -o Hdhv3.changeID.gtf
    this->paramlist.clear();
    this->paramlist.append(gffFile);
    this->paramlist.append("-T");
    this->paramlist.append("-o");
    this->paramlist.append(outFile);

    return true;
}

bool Annovar::gtfToGenePred(QString gtfFile, QString outFile)
{
    if (gtfFile.isNull() || outFile.isNull())
    {
        return false;
    }

    // gtfToGenePred -genePredExt Hdhv3.changeID.gtf Hdhv3_refGene.txt
    this->paramlist.clear();
    this->paramlist.append("-genePredExt");
    this->paramlist.append(gtfFile);
    this->paramlist.append(outFile);

    return true;
}

bool Annovar::retrieveSeqFromFasta(QString refGeneFile, QString seqFile, QString outFile)
{
    if (refGeneFile.isNull() || seqFile.isNull() || outFile.isNull())
    {
        return false;
    }

    /*
       perl retrieve_seq_from_fasta.pl --format refGene --seqfile hdhdb/Hdhv3.changeID.fa
       hdhdb/Hdhv3_refGene.txt --outfile hdhdb/Hdhv3_refGeneMrna.fa
    */
    this->paramlist.clear();
    this->paramlist.append("--format");
    this->paramlist.append("refGene");
    this->paramlist.append("--seqfile");
    this->paramlist.append(seqFile);
    this->paramlist.append(refGeneFile);
    this->paramlist.append("--outfile");
    this->paramlist.append(outFile);

    return true;
}
