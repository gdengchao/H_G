#ifndef POPLDDECAY_H
#define POPLDDECAY_H

#include <QStringList>
#include <QTextStream>
#include <QFileInfo>
#include <QFile>

class PopLDdecay
{
public:
    PopLDdecay();
    QStringList getParamList(void);    // Every param can't have any space.
    QString getParamString(void);
    void setParam(QStringList param);
    void setParam(QString param);
    QString getToolPath(void);

    bool runLD(QString _genotype, QString out);
    bool preGenotype(QString ped, QString map, QString out);
    bool plotLD(QString in, QString out);

    QStringList makeKeepFromTranspose(QString tfam);

private:
    const QString toolpath;
    QStringList paramlist;
};

#endif // POPLDDECAY_H
