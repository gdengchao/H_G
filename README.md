# H_G

1. Phenotype

.phe:   FID IID PHE
else (Multi phenotype, make .phe):   FID IIF PHE1 PHE2


2. Genotype, Map

.vcf        ----plink: .ped, .map           plink --vcf vcfFile [--maf maf] [--geno ms] --recode12 --out out
            ----gemma: .bed, .fam, .bim     plink --vcf vcfFile [--maf maf] [--geno ms] --make-bed --out out
            ----emmax: .tped, .tfam         plink --vcf vcfFile [--maf maf] [--geno ms] --recode12 --transpose --out out

.ped, .map  ----plink: .ped, .map
            ----gemma: .bed, .fam, .bim     plink --ped .ped --map .map [--maf maf] [--geno ms] --make-bed --out out
            ----emmax: .tped, .tfam         plink --ped pedFile --map mapFile [--maf maf] [--geno ms] --recode12 --transpose --out out

3. Kiniship

gemma, emmax: make phenotype before analysis when no phenotype file input.
gemma: gemma -bfile 222_filter1 -gk 1 -o 222_filter1
emmax: emmax-kin -v -h -d 10 222_filter1_rec12 

4. Covariate

plink: --covar  covariate
gemma: -c       covariate
emmax: -c       covariate

5. Association

plink: plink --ped .ped --map .map --assoc --pheno phenotype [--covar covariate] --linear   [--maf maf] [--geno ms]	(For scalar)
                                                                                 --logistic				(For case control)

gemma: (Pre: replace "NA" to -9, then complete .fam)    .fam -- FID IID PID MID Sex 1 PHE
    LMM(Linear Mixed Models):    gemma -bfile geno_pre -k geno_pre.cXX.txt -lmm 1 -n 2 -o out
    BSLMM(Bayesian Sparse Linear Mixed Model):   gemma -bfile geno_pre -bslmm 1 -n 2 -o out

emmax: emmax -v -d 10 -t geno_pre_tfile -p phenotype -k kinship -o out
