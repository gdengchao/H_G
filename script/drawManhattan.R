    args = commandArgs(T)
# "a.assoc.linear"

path = args[1]
# pathvec = as.vector(unlist(strsplit(path,  "/"))) 
# 
# index = which(pathvec == "")
# pathvec = pathvec[-index]
# 
# leng = length(pathvec)
# 
# baseName = pathvec[length(pathvec)]
# pathvec = pathvec[-length(pathvec)]
# abPath = paste("/", paste(pathvec, collapse = "/"), sep = "")


# setwd(abPath) 
data <- read.table(path, header = T)

data$SNP<-as.character(data$SNP)
data$CHR<-as.integer(data$CHR)
newData<-na.omit(data)

library("qqman")
manhattan(newData)

# d = read.table("/home/chao/Desktop/hapmap1/a.assoc.linear", header = T)