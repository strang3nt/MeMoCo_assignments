# library
library(ggplot2)

# basic scatterplot
data <- read.csv('tsp_results.csv', colClasses=c(nodes="character"))
ggplot(data, aes(x=factor(nodes, level=c('10', '20', '40', '80', '100')), y=meanCpuTime, group = 1)) +
    scale_x_discrete("Nodes")+
    geom_point() +
    ggtitle("Nodes against CPU time") +
    ylab("CPU time (seconds)") +
    geom_smooth()
