#include "phenoselector.h"

PhenoSelector::PhenoSelector()
{

}

PhenoSelector::PhenoSelector(QStringList const selectedPheno, QStringList const excludedPheno)
{
    if (!selectedPheno.isEmpty())
    {
        this->selectedPhenoList = selectedPheno;
    }

    if (!excludedPheno.isEmpty())
    {
        this->excludedPhenoList = excludedPheno;
    }
}

void PhenoSelector::setSelectedPheno(QStringList const list)
{
    if (!list.isEmpty())
    {
        this->selectedPhenoList = list;
    }
}

QStringList PhenoSelector::getSelectedPheno(void)
{
    return this->selectedPhenoList;
}

void PhenoSelector::setExcludedPheno(QStringList const list)
{
    if (!list.isEmpty())
    {
        this->excludedPhenoList = list;
    }
}

QStringList PhenoSelector::getExcludedPheno(void)
{
    return this->excludedPhenoList;
}

void PhenoSelector::selectPheno(QStringList pheno)
{
    if (!pheno.isEmpty())
    {
        for (auto item : pheno)
        {
            this->selectedPhenoList.append(item);
            this->excludedPhenoList.removeOne(item);
        }
    }
}

void PhenoSelector::excludePheno(QStringList pheno)
{
    if (!pheno.isEmpty())
    {
        for (auto item : pheno)
        {
            this->selectedPhenoList.removeOne(item);
            this->excludedPhenoList.append(item);
        }
    }
}

void PhenoSelector::selectAllPheno(void)
{
    if (!this->excludedPhenoList.isEmpty())
    {
        this->selectedPhenoList.append(excludedPhenoList);
        this->excludedPhenoList.clear();
    }
}

void PhenoSelector::excludeAllPheno(void)
{
    if (!this->selectedPhenoList.isEmpty())
    {
        this->excludedPhenoList.append(this->selectedPhenoList);
        this->selectedPhenoList.clear();
    }
}
