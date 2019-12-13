#include "workdirectory.h"

WorkDirectory::WorkDirectory()
{

}

void WorkDirectory::setModuleName(QString const name)
{
    this->moduleName = name;
}

QString WorkDirectory::getModuleName(void)
{
    return this->moduleName;
}

void WorkDirectory::setOutputDirectory(QString const dir)
{
    this->outputDirectory = dir;
}

QString WorkDirectory::getOutputDirectory(void)
{
    return this->outputDirectory;
}
