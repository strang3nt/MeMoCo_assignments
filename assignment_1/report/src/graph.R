# library
library(ggplot2)

data <- read.csv('tsp_results.csv')

visual1 <- data.frame(data$nodes, data$cpuTime)
visual2 <- data.frame(data$nodes, data$userTime)
visual1$Mode <- "CPU time"
visual2$Mode <- "User time"

names(visual1)[2] = "time"
names(visual2)[2] = "time"
visual <- rbind(visual1, visual2)
names(visual)[1] = "nodes"

base_breaks <- function(n = 10){
    function(x) {
        axisTicks(log10(range(x, na.rm = TRUE)), log = TRUE, n = n)
    }
}

p <- ggplot(visual, aes(x=nodes, y=time, group=Mode, col=Mode, fill=Mode)) +
    xlab("Nodes")+
    scale_x_continuous(breaks =c(10, 20, 40, 80, 100)) +
    geom_point() +
    ylab("Time (seconds)") +
    geom_smooth()

pLog <- p +
    ggtitle("Execution time, log scale") + 
    scale_y_log10(breaks = base_breaks(), labels = prettyNum)
ggsave(plot = pLog, width = 6, height = 6, dpi = 300, filename = "cpuAndUserTimeLog.pdf")

pLin <- p +
    ggtitle("Execution time")

ggsave(plot = pLin, width = 6, height = 4, dpi = 300, filename = "cpuAndUserTime.pdf")
