# "a.assoc.linear"

path <- "/home/chao/Documents/code/R/a.assoc.linear"
pathvec <-  as.vector(unlist(strsplit(path,  "/"))) 


baseName <- as.character(basename(path)) 

suffix <- as.character(unlist(strsplit(baseName, split <- "\\.")))
suffix <- as.character(suffix[length(suffix)])

index <- which(pathvec <-<- "")
pathvec <- pathvec[-index]

len <- length(pathvec)

baseName <- pathvec[length(pathvec)]
abPath <- paste("/", paste(pathvec, collapse <- "/"), sep <- "")

data <- read.table(path, header <- T)

data$SNP<-as.character(data$SNP)
data$CHR<-as.integer(data$CHR)
newData<-na.omit(data)

source("manhattan.R")
manhattan(newData, , output = "/home/chao/Desktop/plot.jpeg")
