#ifndef GCTA_H
#define GCTA_H

#include <QStringList>

class Gcta
{
public:
    Gcta();
    QStringList getParamList(void);    // Every param can't have any space.
    QString getParamString(void);
    void setParam(QStringList param);
    void setParam(QString param);
    QString getToolPath(void);

    bool makeGRM(QString genoBaseName, QString out);
    bool runPCA(QString grmBaseName, int PCs, int threads, QString out);
private:
    const QString toolpath;
    QStringList paramlist;
};

#endif // GCTA_H
