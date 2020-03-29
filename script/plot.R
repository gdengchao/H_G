
getDataFromSciNotation <- function(scinota = "1E2")
{
    vec = as.vector(unlist(strsplit(tolower(scinota), "", fixed = T)))
    eindex = grep('e', vec)
    base = as.integer(substr(scinota, 1, eindex-1))
    expo = as.integer(substr(scinota, eindex+1, nchar(scinota)))
    ret = base * 10 ^ expo
}

drawManhattan <- function(args)
{
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

drawQQplot <- function(args)
{
    inData = args[1]
    output =args[2]
    
    write(inData, stdout())
    write(output, stdout())
    
    data <- read.table(inData, header = T)
    data <- unlist(c(data)) 
    
    newData<-na.omit(data)
    
    ### NOTICE the path of manhattan.R
    source("/home/chao/Documents/code/H_G/script/qq.R")
    qq(newData, output)
}

main <- function()
{
    # Get parameter from args.
    args = commandArgs(T)
    
    if (args[1] == "manhattan")
    {
        drawManhattan(args[-1])
    }
    if (args[1] == "qqplot")
    {
        drawQQplot(args[-1])
    }
    # "a.assoc.linear"
}

# col.Rainbow=rainbow(ncolor+1)[thecolor]     	
# col.FarmCPU=rep(c("#CC6600","deepskyblue","orange","forestgreen","indianred3"),ceiling(numCHR/5))
# col.Rushville=rep(c("orangered","navyblue"),ceiling(numCHR/2))   	
# col.Congress=rep(c("deepskyblue3","firebrick"),ceiling(numCHR/2))
# col.Ocean=rep(c("steelblue4","cyan3"),ceiling(numCHR/2)) 		
# col.PLINK=rep(c("gray10","gray70"),ceiling(numCHR/2)) 		
# col.Beach=rep(c("turquoise4","indianred3","darkolivegreen3","red","aquamarine3","darkgoldenrod"),ceiling(numCHR/5))
# #col.Oceanic=rep(c(	'#EC5f67',	'#F99157',	'#FAC863',	'#99C794',	'#5FB3B3',	'#6699CC',	'#C594C5',	'#AB7967'),ceiling(numCHR/8))
# #col.Oceanic=rep(c(	'#EC5f67',		'#FAC863',	'#99C794',		'#6699CC',	'#C594C5',	'#AB7967'),ceiling(numCHR/6))
# col.Oceanic=rep(c(	'#EC5f67',		'#FAC863',	'#99C794',		'#6699CC',	'#C594C5'),ceiling(numCHR/5))
# col.cougars=rep(c(	'#990000',		'dimgray'),ceiling(numCHR/2))

main()
