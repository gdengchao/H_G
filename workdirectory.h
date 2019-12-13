#ifndef WORKDIRECTORY_H
#define WORKDIRECTORY_H

#include <QString>

class WorkDirectory
{
public:
    WorkDirectory();
    WorkDirectory(QString const name, QString const dir);

    void setModuleName(QString const name);
    QString getModuleName(void);
    void setOutputDirectory(QString const dir);
    QString getOutputDirectory(void);

private:
    QString moduleName;
    QString outputDirectory;
};

#endif // WORKDIRECTORY_H
