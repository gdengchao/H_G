#include "assoctool.h"

AssocTool::AssocTool()
{

}

QStringList AssocTool::getSupportedModel(void)
{
    return this->model;
}

QString AssocTool::getToolPath()
{
    return this->toolpath;
}
