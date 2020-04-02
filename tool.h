#ifndef TOOL_H
#define TOOL_H

#include <QStringList>

// Just for Association.
class Tool
{
public:
    Tool();

    QStringList getParamList(void);    // Every param can't have any space.
    QString getParamString(void);
    void setParam(QStringList param);
    void setParam(QString param);
    QStringList getSupportedModel(void);
    QString getToolPath(void);

protected:
    const QString toolpath;
    QStringList model;
    QStringList paramlist;
};

#endif // TOOL_H
