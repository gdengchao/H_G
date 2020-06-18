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
            qApp->processEvents();
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
    QMap<QString, QString> snpID;    // SNPs need to extract.
    while (!pvalFileStream.atEnd())
    {
        qApp->processEvents();
        QStringList curLine = pvalFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        snpID.insert(curLine[0], curLine[0]+"\t"+curLine[2]);   // cut the SNP_ID and p-value
    }

    QFile mapFile(mapFilePath);
    QFile outFile(outFilePath);
    if (!mapFile.open(QIODevice::ReadOnly) ||
        !outFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    // traverse map file, extract SNP.
    QTextStream mapFileStream;
    QTextStream outFileStream;
    while (!mapFile.atEnd())
    {
        QStringList curLine = mapFileStream.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (snpID.find(curLine[1]) != snpID.end())
        {
            outFileStream << snpID[curLine[1]] << "chr" << curLine[0] << "\t" << curLine[3] << endl;
        }

        qApp->processEvents();
    }

    return true;
}

/**
 * @brief FuncAnnotator::complExoSnpInfo
 *      Add gene ID and mutation type to SNP in exonic
 *      (Except the annotatin is unknown of SNP in .exonic_variant_function)
 * @param snpPosFilePath        :SNP_ID P-VALUE CHR BP
 * @param exValFuncFilePath     :.exonic_variant_function
 * @param outFilePath           : gene_ID mutation_type (+ snpPosFile)
 * @return
 */
bool FuncAnnotator::complExoSnpInfo(QString const snpPosFilePath, QString const exValFuncFilePath, QString const outFilePath)
{

}
