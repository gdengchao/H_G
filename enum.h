#ifndef ENUM_H
#define ENUM_H

enum EFileType
{
    phenotype = 0,
    genotype = 1,
    map = 2,
    covariate = 3,
    kinship = 4,
};

enum ETools
{
    plink,
    emmax,
    gemma,
    gapit,
};

enum EModel
{
    glm,
    mlm,
    lr,
};



#endif // ENUM_H
