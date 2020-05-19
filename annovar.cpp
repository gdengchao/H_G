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

bool Annovar::tableAnnovar(QString avinput, QString refGeneDir, QString refGenePrefix, QString out)
{
    if (avinput.isNull() || refGeneDir.isNull() || refGenePrefix.isNull() || out.isNull())
    {
        return false;
    }

    QFile file;
    file.setFileName(refGeneDir + "/" + refGenePrefix + "_refGeneMrna.fa");
    if (!file.exists())
    {
        return false;
    }
    file.setFileName(refGeneDir + "/" + refGenePrefix + "_refGene.txt");

    if (!file.exists())
    {
        return false;
    }

    // perl ../table_annovar.pl  test.avinput sp/ --buildver SP --outfile myanno --protocol refGene --operation g
    this->paramlist.clear();
    this->paramlist.append(avinput);
    this->paramlist.append(refGeneDir);
    this->paramlist.append("--buildver");
    this->paramlist.append(refGenePrefix);
    this->paramlist.append("--outfile");
    this->paramlist.append(out);
    this->paramlist.append("--protocol");
    this->paramlist.append("refGene");
    this->paramlist.append("--operation");
    this->paramlist.append("g");

    return true;
}

bool Annovar::annotateVariation(QString avinput, QString refGeneDir, QString refGenePrefix, QString out)
{
    if (avinput.isNull() || refGeneDir.isNull() || refGenePrefix.isNull() || out.isNull())
    {
        return false;
    }

    QFile file;
    file.setFileName(refGeneDir + "/" + refGenePrefix + "_refGeneMrna.fa");
    if (!file.exists())
    {
        return false;
    }
    file.setFileName(refGeneDir + "/" + refGenePrefix + "_refGene.txt");

    if (!file.exists())
    {
        return false;
    }

    // ../annotate_variation -out hdhdb/anno -build Hdhv3 input_for_annovar hdhdb
    this->paramlist.clear();
    this->paramlist.append("--out");
    this->paramlist.append(out);
    this->paramlist.append("--build");
    this->paramlist.append(refGenePrefix);
    this->paramlist.append(avinput);
    this->paramlist.append(refGeneDir);

    return true;
}
