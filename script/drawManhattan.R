
getDataFromSciNotation <- function(scinota = "1E2")
{
    vec = as.vector(unlist(strsplit(tolower(scinota), "", fixed = T)))
    eindex = grep('e', vec)
    base = as.integer(substr(scinota, 1, eindex-1))
    expo = as.integer(substr(scinota, eindex+1, nchar(scinota)))
    ret = base * 10 ^ expo
}

main <- function()
{
    # Get parameter from args.
    args = commandArgs(T)
    # "a.assoc.linear"
    
    inData = args[1]
    out =args[2]
    gwline = args[3]
    sgline = args[4]
    
    genomewide = -log10(getDataFromSciNotation(gwline))
    suggest = -log10(getDataFromSciNotation(sgline))
    
    write(inData, stderr())
    write(out, stderr())
    write(gwline, stderr())
    write(sgline, stderr())
    
    data <- read.table(inData, header = T)
    
    data$SNP<-as.character(data$SNP)
    data$CHR<-as.integer(data$CHR)
    newData<-na.omit(data)
    
    ### NOTICE the path of manhattan.R
    source("/home/chao/Documents/code/H_G/script/manhattan.R") #/home/chao/Documents/code/H_G/script/manhattan.R
    manhattan(newData, output = out, suggestiveline = suggest, genomewideline = genomewide)
}

main()
