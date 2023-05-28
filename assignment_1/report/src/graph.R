# library
library(ggplot2)
library(patchwork)
# basic scatterplot
data <- read.csv('tsp_results.csv', colClasses=c(nodes="character"))

visual1 <- data.frame(data$nodes, data$cpuTime)
visual2 <- data.frame(data$nodes, data$userTime)
visual1$Mode <- "CPU time"
visual2$Mode <- "User time"

names(visual1)[2] = "time"
names(visual2)[2] = "time"


visual <- rbind(visual1, visual2)
names(visual)[1] = "nodes"
print(visual)

base_breaks <- function(n = 10){
    function(x) {
        axisTicks(log10(range(x, na.rm = TRUE)), log = TRUE, n = n)
    }
}

ggplot(visual, aes(x=factor(nodes, level=c('10', '20', '40', '80', '100')), y=time, group=Mode, col=Mode, fill=Mode)) +
    scale_x_discrete("Nodes")+
    geom_point() +
    ggtitle("Execution time, log scale") +
    # ggtitle("Execution time") +
    ylab("Time (seconds)") +
    scale_y_log10(breaks = base_breaks(), labels = prettyNum) +
    geom_smooth()
