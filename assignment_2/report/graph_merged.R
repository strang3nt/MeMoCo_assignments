# library
library(ggplot2)

# basic scatterplot
visual1 <- read.csv('tsp_results.csv', colClasses=c(nodes="character"))
visual2 <- read.csv('tsp_results_1.csv', colClasses=c(nodes="character"))

names(visual1)[9] ="cpuTime"
visual1$group <- "Lin-Kernighan"
visual2$group <- "MILP"
common <- intersect(colnames(visual1), colnames(visual2))
visuals = rbind(visual1[common], visual2[common])

base_breaks <- function(n = 10){
    function(x) {
        axisTicks(log10(range(x, na.rm = TRUE)), log = TRUE, n = n)
    }
}

ggplot(visuals, aes(x=factor(nodes, level=c('10', '20', '40', '80', '100')), y=cpuTime, group=group, col=group, fill=group)) +
    scale_x_discrete("Nodes")+
    geom_point() +
    ggtitle("Nodes against CPU time") +
    ylab("CPU time (seconds)") +
    scale_y_log10(breaks = base_breaks(), labels = prettyNum) +
    geom_smooth()

