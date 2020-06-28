#include "funcannotator.h"

FuncAnnotator::FuncAnnotator()
{

}

/**
 * @brief FunctionalAnnotator::filterSNP
 *      select SNP whose p-value bigger than threshold to out file.
 *      the threhold is -log(thBase * 10 ^ thExpo)
 * @param pvalFile      a file the last column is p value.
 * @param thBase        base of threshold
 * @param thExpo        exponent of threshold
 * @param out           a file to store SNP which is selected.
 * @return              fail(false) or success(true).
 */
bool FuncAnnotator::filterSNP(QString const pvalFilePath, QString const thBase,
                              QString const thExpo, QString const outFilePath)
{
    if (pvalFilePath.isNull() || thBase.isNull() || thExpo.isNull() || outFilePath.isNull())
    {
        return false;
    }

    int thresholdBase = thBase.toInt();
    int thresholdExpo = thExpo.toInt();
    float threshold = -log10f(thresholdBase*pow10f(thresholdExpo));

    QFile pvalFile(pvalFilePath);
    QFile outFile(outFilePath);

    if (!pvalFile.open(QIODevice::ReadOnly) ||
        !outFile.open(QIODevice::ReadWrite))
    {
        return false;
    }
    QTextStream pvalFileStream(&pvalFile);
    QTextStream outFileStream(&outFile);

    while (!pvalFileStream.atEnd())
    {
        QString curLine = pvalFileStream.readLine();
        QStringList curLineList = curLine.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (-log10f(curLineList.constLast().toFloat()) >= threshold)
        {
            outFileStream << curLine << endl;
        }
    }
    pvalFile.close();
    outFile.close();

    return true;
}

/**
 * @brief extractPos
 *      Extrac position of SNP and save to outFile.(reserve p-value)
 * @param pvalFilePath  p-value file (Have been fitered)
 * @param mapFilePath   map file(There position info of SNP)
 * @param outFilePath   output file(SNP_ID, p-value, CHR, BP)
 * @return
 */
bool FuncAnnotator::extractPos(QString const pvalFilePath, QString const mapFilePath, QString const outFilePath)
{
    if (pvalFilePath.isNull() || mapFilePath.isNull() || outFilePath.isNull())
    {
        return false;
    }

    // Get the SNP which need to extract and save to snpID(QSet).
    QFile pvalFile(pvalFilePath);
    if (!pvalFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QTextStream pvalFileStream(&pvalFile);
    QMap<QString, QString> snpIDMap;    // SNPs need to extract.
    while (!pvalFileStream.atEnd())
    {
        QStringList curLine = pvalFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        snpIDMap.insert(curLine[0], curLine[0]+"\t"+curLine[2]);   // cut the SNP_ID and p-value
    }

    QFile mapFile(mapFilePath);
    QFile outFile(outFilePath);
    if (!mapFile.open(QIODevice::ReadOnly) ||
        !outFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    // traverse map file, extract SNP.
    QTextStream mapFileStream(&mapFile);
    QTextStream outFileStream(&outFile);
    while (!mapFile.atEnd())
    {
        QStringList curLine = mapFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (snpIDMap.find(curLine[1]) != snpIDMap.end())
        {
            outFileStream << snpIDMap[curLine[1]] << "\tchr" << curLine[0] << "\t" << curLine[3] << endl;
        }
    }

    return true;
}

/**
 * @brief FuncAnnotator::complExoSnpInfo
 *      Add gene ID and mutation type to SNP in exonic
 *      (Except the annotatin is unknown of SNP in .exonic_variant_function)
 * @param snpPosFilePath        :SNP_ID P-VALUE CHR BP
 * @param exValFuncFilePath     :.exonic_variant_function
 * @param outFilePath           : gene_ID, mutation_type, snpPosFile(SNP_ID P-VALUE CHR BP),other info
 * @return
 */
bool FuncAnnotator::complExoSnpInfo(QString const snpPosFilePath, QString const exVarFuncFilePath, QString const outFilePath)
{
    if (snpPosFilePath.isNull() ||
        exVarFuncFilePath.isNull() ||
        outFilePath.isNull())
    {
        return false;
    }

    ///////////////////////////////////////
    ////////    Read snp position  ////////
    ///////////////////////////////////////
    QFile snpPosFile(snpPosFilePath);
    if (!snpPosFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QTextStream snpPosFileStream(&snpPosFile);
    QMap<QString, QStringList> snpPosInfoMap;       // Save snp position and info.
    while (!snpPosFileStream.atEnd())
    {
        QStringList curLine = snpPosFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QString chr_bp = curLine[2] + "_" + curLine[3];
        snpPosInfoMap.insert(chr_bp, curLine);
    }

    /////////////////////////////////////////////////////
    ////////    Capture exonic by snp position.  ////////
    /////////////////////////////////////////////////////
    QFile exValFuncFile(exVarFuncFilePath);
    QFile outFile(outFilePath);
    if (!exValFuncFile.open(QIODevice::ReadOnly) ||
        !outFile.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QTextStream outFileStream(&outFile);
    QTextStream exValFuncFileStream(&exValFuncFile);
    while (!exValFuncFileStream.atEnd())
    {
        QStringList curLine = exValFuncFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QString chr_bp = curLine[4] + "_" + curLine[5];
        if (snpPosInfoMap.find(chr_bp) == snpPosInfoMap.end())
        {
            continue;
        }
        // aminoAcidChange: Amino acid changes
        //     contains the gene name, the transcript identifier and the sequence change in the corresponding transcript
        QString aminoAcidChange = curLine[3];
        int commaCounter = aminoAcidChange.count(',');
        if (commaCounter == 1)
        {
            QRegExp regExp("([^:]*):(.*),");    //  Obj data: "HDH_G00730:HDH_T00730:exon1:c.A132G:p.T44T,"
            int pos = regExp.indexIn(aminoAcidChange);
            if (pos == -1)
            {   // Match string error.
                return false;
            }
            QStringList list = regExp.capturedTexts();
            QString subStr_1 = regExp.cap(1);  // "HDH_G00730"
            QString subStr_2 = regExp.cap(2);  // "HDH_T00730:exon1:c.A132G:p.T44T,"
            outFileStream << subStr_1 << "\t" << curLine[1] << "\t"
                          << snpPosInfoMap[chr_bp].join("\t") << "\t" << subStr_2 << endl;
        }
        else if (commaCounter == 2)
        {   // Assume there are two annotation.()
            QStringList aminoAcidChangeList = aminoAcidChange.split(",", QString::SkipEmptyParts);

            // The first annotation
            QRegExp regExp("([^:]*):(.*)");    //  Obj data: "HDH_G00730:HDH_T00730:exon1:c.A132G:p.T44T"
            int pos = regExp.indexIn(aminoAcidChangeList[0]);
            if (pos == -1)
            {   // Match string error.
                return false;
            }
            QStringList list = regExp.capturedTexts();
            QString subStr_1 = regExp.cap(1);  // "HDH_G00730"
            QString subStr_2 = regExp.cap(2);  // "HDH_T00730:exon1:c.A132G:p.T44T,"

            // $hash1{$tmp}=[$1, $F[1], $2];
            // $hash1{$tmp}[0]\t$hash1{$tmp}[1]\t$_\t$hash1{$tmp}[2]
            outFileStream << subStr_1 << "\t" << curLine[1] << "\t"
                          << snpPosInfoMap[chr_bp].join("\t") << "\t" << subStr_2 << endl;

            //  The second annotation
            pos = regExp.indexIn(aminoAcidChangeList[1]);
            if (pos == -1)
            {   // Match string error.
                return false;
            }
            list = regExp.capturedTexts();
            subStr_1 = regExp.cap(1);  // "HDH_G00730"
            subStr_2 = regExp.cap(2);  // "HDH_T00730:exon1:c.A132G:p.T44T,"
            // $hash2{$tmp}[0]\t$hash2{$tmp}[1]\t$_\t$hash2{$tmp}[2]
            // snp2ndInfoMap.insert(chr_bp, QStringList()<<subStr_1<<curLine[1]<<subStr_2);
            outFileStream << subStr_1 << "\t" << curLine[1] << "\t"
                          << snpPosInfoMap[chr_bp].join("\t") << "\t" << subStr_2 << endl;
        }
    }

    return true;
}

/**
 * @brief FuncAnnotator::complNonExoSnpInfo
 *          Add the gene ID and region before the SNP loci located outside of the gene exon,
 *      and add the SNP relative to the gene information in the last column.
 * @param exonicPosFilePath     :gene_ID, mutation_type, snpPosFile(SNP_ID P-VALUE CHR BP),other info
 * @param snpPosFilePath        :SNP_ID P-VALUE CHR BP
 * @param varFuncFilePath       :.variant_function
 * @param outFilePath           :gene_ID, mutation_type, snpPosFile:(SNP_ID, P-VALUE, CHR, BP), distance
 * @return
 */
bool FuncAnnotator::complNonExoSnpInfo(QString const exonicPosFilePath, QString const snpPosFilePath,
                                       QString const varFuncFilePath, QString const outFilePath)
{   // Wait for reconstruct.
    if (exonicPosFilePath.isNull() ||
        snpPosFilePath.isNull() ||
        varFuncFilePath.isNull() ||
        outFilePath.isNull())
    {
        return false;
    }

    ////////////////////////////////////////////
    //////////    Read exonic snp ID  //////////
    ////////////////////////////////////////////
    QFile exonicPosFile(exonicPosFilePath);
    if (!exonicPosFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QTextStream exonicPosFileStream(&exonicPosFile);
    QSet<QString> exonicSnpIDSet;
    while (!exonicPosFileStream.atEnd())
    {
        QStringList curLine = exonicPosFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        exonicSnpIDSet.insert(curLine[2]);
    }

    ///////////////////////////////////////
    ////////    Read snp position  ////////
    ///////////////////////////////////////
    QFile snpPosFile(snpPosFilePath);
    if (!snpPosFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QTextStream snpPosFileStream(&snpPosFile);
    QMap<QString, QStringList> snpPosInfoMap;       // Save snp position and info.
    while(!snpPosFileStream.atEnd())
    {
        QStringList curLine = snpPosFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (exonicSnpIDSet.find(curLine[0]) != exonicSnpIDSet.end())
        {   // Exclude snp in exonic.
            continue;
        }
        QString chr_bp = curLine[2]+"_"+curLine[3];
        snpPosInfoMap.insert(chr_bp, curLine);
    }

    /////////////////////////////////////////////////////
    ////////    Capture exonic by snp position.  ////////
    /////////////////////////////////////////////////////
    QFile varFuncFile(varFuncFilePath);
    QFile outFile(outFilePath);
    if (!varFuncFile.open(QIODevice::ReadOnly) ||
        !outFile.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QTextStream varFuncFileStream(&varFuncFile);
    QTextStream outFileStream(&outFile);
    while(!varFuncFileStream.atEnd())
    {
        QStringList curLine = varFuncFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (curLine[0] == "intergenic")
        {   // Not to deal with.
            continue;
        }
        QString chr_bp = curLine[2]+"_"+curLine[3];
        if (snpPosInfoMap.find(chr_bp) == snpPosInfoMap.end())
        {   // Not to deal with.
            continue;
        }

        int commaCounter = curLine[1].count(',');
        int semiCounter = curLine[1].count(';');

        if (commaCounter < 1 && semiCounter < 1)
        {//curLine[0]:intronic  curLine[1]:HDH_G00724
            // gene: Gene_ID(distance) ---> Gene_ID, distance
            QStringList geneAnno = curLine[1].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
            outFileStream << geneAnno[0] << "\t" << curLine[0] << "\t"
                          << snpPosInfoMap[chr_bp].join("\t") << "\t"
                          << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
        }
        else if (commaCounter == 1 && semiCounter == 1)
        {//curLine[0]:upstream;downstream   curLine[1]:HDH_G00991,HDH_G00992(dist=242);HDH_G00993(dist=838)
            QStringList gene = curLine[1].split(";", QString::SkipEmptyParts);
            QStringList gene_1st = gene[0].split(",", QString::SkipEmptyParts);
            QStringList gene_2nd = gene[1].split(",", QString::SkipEmptyParts);
            QStringList snpRegion = curLine[0].split(";", QString::SkipEmptyParts);
            if (gene_1st.length() < 2)
            {   // A;B,C
                QStringList geneAnno = gene_1st[0].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
                outFileStream << geneAnno[0] << "\t" << snpRegion[0] << "\t"
                              << snpPosInfoMap[chr_bp].join("\t") << "\t"
                              << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
                geneAnno = gene_2nd[0].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
                outFileStream << geneAnno[0] << "\t" << snpRegion[1] << "\t"
                              << snpPosInfoMap[chr_bp].join("\t") << "\t"
                              << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
                geneAnno = gene_2nd[1].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
                outFileStream << geneAnno[0] << "\t" << snpRegion[1] << "\t"
                              << snpPosInfoMap[chr_bp].join("\t") << "\t"
                              << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
            }
            else if (gene_2nd.length() < 2)
            {   // A,B;C
                QStringList geneAnno = gene_1st[0].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
                outFileStream << geneAnno[0] << "\t" << snpRegion[0] << "\t"
                              << snpPosInfoMap[chr_bp].join("\t") << "\t"
                              << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
                geneAnno = gene_1st[1].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
                outFileStream << geneAnno[0] << "\t" << snpRegion[0] << "\t"
                              << snpPosInfoMap[chr_bp].join("\t") << "\t"
                              << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
                geneAnno = gene_2nd[0].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
                outFileStream << geneAnno[0] << "\t" << snpRegion[1] << "\t"
                              << snpPosInfoMap[chr_bp].join("\t") << "\t"
                              << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
            }
        }
        else if (commaCounter == 1)
        {   // curLine[0]:intergenic    curLine[1]:HDH_G00724(dist=1008),HDH_G00725(dist=39868)
            QStringList geneAnno = curLine[1].split(",", QString::SkipEmptyParts);
            QStringList gene_1st = geneAnno[0].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
            outFileStream << gene_1st[0] << "\t" << curLine[0] << "\t"
                          << snpPosInfoMap[chr_bp].join("\t") << "\t"
                          << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
            QStringList gene_2nd = geneAnno[1].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
            outFileStream << gene_2nd[0] << "\t" << curLine[0] << "\t"
                          << snpPosInfoMap[chr_bp].join("\t") << "\t"
                          << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
        }
        else if (semiCounter == 1)
        {   // curLine[0]:upstream;downstream     curLine[1]:HDH_G00745(dist=401);HDH_G00744(dist=386)
            QStringList gene = curLine[1].split(";", QString::SkipEmptyParts);
            QStringList snpRegion = curLine[0].split(";", QString::SkipEmptyParts);
            QStringList geneAnno = gene[0].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
            outFileStream << geneAnno[0] << "\t" << snpRegion[0] << "\t"
                          << snpPosInfoMap[chr_bp].join("\t") << "\t"
                          << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
            geneAnno = gene[1].split(QRegExp("\\(|\\)"), QString::SkipEmptyParts);
            outFileStream << geneAnno[0] << "\t" << snpRegion[1] << "\t"
                          << snpPosInfoMap[chr_bp].join("\t") << "\t"
                          << (geneAnno.length() < 2 ? "" : geneAnno[1]) << endl;
        }
    }

    return true;
}

/**
 * @brief FuncAnnotator::complFuncAnnoInfo
 *          Add functional annotation information according to the functional annotation database
 * @param exonicPosFilePath     :gene_ID, mutation_type, snpPosFile(SNP_ID P-VALUE CHR BP),other info
 * @param nonExonicPosFile      :gene_ID, mutation_type, snpPosFile:(SNP_ID, P-VALUE, CHR, BP), distance
 * @param baseFilePath          :annotation database file.
 * @param outFilePath           :annotation result file.
 * @return
 */
bool FuncAnnotator::complFuncAnnoInfo(QString const exonicPosFilePath, QString nonExonicPosFilePath,
                                      QString baseFilePath, QString outFilePath)
{
    if (exonicPosFilePath.isNull() ||
        nonExonicPosFilePath.isNull() ||
        baseFilePath.isNull() ||
        outFilePath.isNull())
    {
        return false;
    }

    QFile exonicPosFile(exonicPosFilePath);
    QFile nonExonicPosFile(nonExonicPosFilePath);
    if (!nonExonicPosFile.open(QIODevice::ReadOnly) ||
        !exonicPosFile.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QTextStream exonicPosFileStream(&exonicPosFile);
    QTextStream nonExonicPosFileStream(&nonExonicPosFile);
    QMap<QString, QStringList> snpPosMap;       // Save both exonicPos and nonExonicPos.
    while (!exonicPosFileStream.atEnd())
    {
        QStringList curLine = exonicPosFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (curLine.length() == 6)
        {
            snpPosMap.insert(curLine[0], QStringList() << curLine << "unknown");
        }
        else
        {
            snpPosMap.insert(curLine[0], curLine);
        }
    }
    while (!nonExonicPosFileStream.atEnd())
    {
        QStringList curLine = nonExonicPosFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (curLine.length() == 6)
        {
            snpPosMap.insert(curLine[0], QStringList() << curLine << "unknown");
        }
        else
        {
            snpPosMap.insert(curLine[0], curLine);
        }
    }

    QFile baseFile(baseFilePath);
    QFile outFile(outFilePath);
    if  (!baseFile.open(QIODevice::ReadOnly) ||
         !outFile.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QTextStream baseFileStream(&baseFile);
    QTextStream outFileStream(&outFile);
    while (!baseFileStream.atEnd())
    {
        QStringList curLine = baseFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        QString id = curLine[0].replace('T', 'G');
        if (snpPosMap.find(id) == snpPosMap.end())
        {
            continue;
        }
        curLine.removeFirst();
        outFileStream << snpPosMap[id].join("\t") << "\t" << curLine.join("\t") << endl;
    }

    return true;
}
