#ifndef PHENOSELECTOR_H
#define PHENOSELECTOR_H

#include <QStringList>

class PhenoSelector
{
public:
    PhenoSelector();
    PhenoSelector(QStringList const selectedPheno, QStringList const exludedPheno);
    void setSelectedPheno(QStringList const list);
    QStringList getSelectedPheno(void);
    void setExcludedPheno(QStringList const list);
    QStringList getExcludedPheno(void);

    void selectPheno(QStringList pheno);
    void excludePheno(QStringList pheno);
    void selectAllPheno(void);
    void excludeAllPheno(void);

private:
    QStringList selectedPhenoList;
    QStringList excludedPhenoList;
};

#endif // PHENOSELECTOR_H
