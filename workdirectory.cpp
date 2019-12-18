#include "workdirectory.h"

WorkDirectory::WorkDirectory()
{

}

void WorkDirectory::setModuleName(QString const name)
{
    if (name.isEmpty())
    {
        this->moduleName = name;
    }
}

QString WorkDirectory::getModuleName(void)
{
    return this->moduleName;
}

void WorkDirectory::setOutputDirectory(QString const dir)
{
    if (dir.isEmpty())
    {
        this->outputDirectory = dir;
    }
}

QString WorkDirectory::getOutputDirectory(void)
{
    return this->outputDirectory;
}
