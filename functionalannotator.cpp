#include "functionalannotator.h"

FunctionalAnnotator::FunctionalAnnotator()
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
bool FunctionalAnnotator::filterSNP(QString pvalFilePath, QString thBase, QString thExpo, QString outFilePath)
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
