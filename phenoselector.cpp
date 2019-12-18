#include "phenoselector.h"

PhenoSelector::PhenoSelector()
{

}

PhenoSelector::PhenoSelector(QStringList const selectedPheno, QStringList const excludedPheno)
{
    if (selectedPheno.isEmpty())
    {
        this->selectedPheno = selectedPheno;
    }

    if (excludedPheno.isEmpty())
    {
        this->excludedPheno = excludedPheno;
    }
}

void PhenoSelector::setSelectedPheno(QStringList const list)
{
    if (list.isEmpty())
    {
        this->selectedPheno = list;
    }
}

QStringList PhenoSelector::getSelectedPheno(void)
{
    return this->selectedPheno;
}

void PhenoSelector::setExcludedPheno(QStringList const list)
{
    if (list.isEmpty())
    {
        this->excludedPheno = list;
    }
}

QStringList PhenoSelector::getExcludedPheno(void)
{
    return this->excludedPheno;
}
