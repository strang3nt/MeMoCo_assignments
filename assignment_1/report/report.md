---
title:  'Models and Methods for Combinatorial Optimization: Assignment 1'
author:
- Alessandro Flori^[Department of Mathematics ``Tullio Levi-Civita'', University of Padua]
institute: 
- Department of Mathematics ``Tullio Levi-Civita''
- University of Padua
date: 17 February, 2023
abstract: |
  The goal of this assignment is to solve 
  the Traveling Salesperson Problem (TSP) for a drilling problem via a Mixed Integer Linear Programming model, and
  study up to which size the problem can be solved in different amounts of time (e.g. 0.1s, 1s, 10s,...). The implementation uses the CPLEX API.
geometry: margin=1in
lang: en-GB
bibliography: references.bibtex
colorlinks: yes
codeBlockCaptions: True
...

# Introduction

A company produces boards with holes used to build electric panels. 
The company wants to minimize the total drilling time for each board, taking into account that the time needed for making an hole is the same and constant for
all the holes. This is an instance of the Traveling Salesperson Problem (TSP).

In this report I show an implementation of a (mixed) integer linear programming model (MILP)
of the TSP. The implementation is written in the 
C++ language and uses the CPLEX Callable Library [@noauthor_ibm_2021]. The goal is to 
study up to which size the problem can be solved in different amounts of time: 0.1 second, 1 second, 10 seconds. 

In the following I show a description of the model. Then [TSP instance for the drilling problem] describes
the how I generated instances for this particular instance of TSP.
Section [Tests] contains a description of the instances used and a study of the implementation's behavior under different sizes of problems.

# MILP model

The model is exactly the one described in [@gavish_travelling_1978].

Sets:

\begin{tabular}{ll}
  $N$ & set of nodes \\
  $A$ & set of edges $(i, j) \in N$. \\
\end{tabular}

Parameters:

\begin{tabular}{lll}
  $c_{ij}$ & $(i,j) \in A$ & const of arc $(i,j)$ \\
  $0$      & $0\in N$      & the arbitrarily selected starting node.
\end{tabular}

Variables:

\begin{tabular}{lll}
  $x_{ij}$ & $j \neq 0, (i,j) \in A$ & flow shipped from $i$ to $j$ \\
  $y_{ij}$ & $(i,j) \in A$           & $1$ if arch $(i, j)$ ships some flow, $0$ otherwise.
\end{tabular}

\setcounter{equation}{0}

Objective:

\begin{align}
  \min \quad
  & \sum_{i,j:(i,j)\in A} c_{ij} y_{ij}
\end{align}

Constraints:

\begin{align}
  \sum_{i:(i,j) \in A} x_{ik}-\sum_{j:(k,j) \in A,j\neq 0} x^k_{ji} & = 1 && \forall k\in N \setminus \{0\} \\
  \sum_{i:(i,j)\in A} y_{ij}&= 1 && \forall i\in N \\
  \sum_{j:(i,j)\in A} y_{ij}&= 1 && \forall j\in N \\
  x_{ij} & \leq (|N| - 1) y_{ij} && \forall (i,j)\in A,j\neq 0 \\
  x_{ij} \in \mathbb{R}_+ & && \forall (i,j)\in A,j\neq 0 \\
  y_{ij} \in \{0,1\} & && \forall (i,j)\in A.
\end{align}

# TSP instances for the drilling problem

## Generation of instances

An ad-hoc solution was created to generate realistic instances for the drilling problem.
Instances are produced via a script.
The instance generator produces files according to the following input:

 1. size of board, height and length
 2. distance between holes coordinates
 3. size of instance and number of instances.

The script fills a two-dimensional plane with as many coordinates as possible, distributed in a grid, where holes are separated by 
the distance input in (2). Then it uses these coordinates to select randomly a number of points, which are then saved in a file, and
this process is repeated with respect to the input provided in step (3).
The files come in a `.tsp` extension, they are named with the following pattern: `userprovidedname_numberofinstance.tsp`.
The following is an example of a file, the first line is the number of nodes, the next lines contain the number of the node,
and its x, y coordinates in the plane:

```
10
1 0 13
2 0 16
3 0 19
4 0 22
5 0 25
6 0 28
7 0 31
8 0 34
9 0 37
10 0 4
```

I decided to use a precision in the order of $10^{-5}$, every input is integer, as well as the distance between points, which is calculated using
the euclidean distance, is rounded to the nearest integer.

A `.tsp` file is then parsed to a graph object, which is then used by the CPLEX API and the MILP implementation.

> Note that a graph is supposed to be complete, each node is connected to each end every other node in the graph.

## A solution for the TSP drilling problem

The results are euclidean distances, which is not a correct answer for the question at hand, 
which is how much time it takes for a mechanical arm to drill
a board. I decided to use distances nonetheless, since they can be easily converted to
time measures, once the drilling speed and the speed of movement of the arm is known.

The unit of measure is $10^{-5}$ meters, which is probably more precise than a real drilling machine would support.
Values are integers.
For example if a solution is $25.167$, it means that the length of the best possible hamiltonian path is $25,167$ centimeters.

# Tests

## Test data

Through the instance generator I generated 5 different types of instances:

 - 10 nodes, size of two-dimensional plane $10\times 10$cm, the distance between nodes in the grid is $1,5$cm
 - 20 nodes, $10\times 15$cm, $1,5$cm
 - 40 nodes, $20\times 15$cm, $1,5$cm
 - 80 nodes, $30\times 20$cm, $2,0$cm
 - 100 nodes, $30\times 25$cm, $2,0$cm.

There are 10 randomly generated instances of each size.

## Test results

> The benchmark suite was executed on a laptop equipped with linux, with 8gb of ram, a 6 cores, 12 threads CPU, a local CPLEX install.

Table \ref{tab:results} shows user and CPU time of the different instances.
The user time is the time it took to obtain a solution for an instance.
The CPU time is the time the test took to complete, taking into account all cores time of use:
CPLEX uses multi-threading to solve linear programming problems, CPU time can be thought as
the total amount of time it would take if the test was executed on a single-core CPU.

| Instance | Nodes | Result | User time (s) | CPU time (s) |
| :--- | ---: | ---: | ---: | ---: |
| 010_s_0 | 10 | 25.167 | 0,04076 | 0,14415 |
| 010_s_1 | 10 | 22.416 | 0,03748 | 0,15791 |
| 010_s_2 | 10 | 29.489 | 0,03324 | 0,17605 |
| 010_s_3 | 10 | 22.950 | 0,03500 | 0,17805 |
| 010_s_4 | 10 | 25.339 | 0,05050 | 0,19526 |
| 010_s_5 | 10 | 27.524 | 0,02911 | 0,11154 |
| 010_s_6 | 10 | 29.547 | 0,05778 | 0,34336 |
| 010_s_7 | 10 | 26.314 | 0,03196 | 0,13062 |
| 010_s_8 | 10 | 23.121 | 0,05941 | 0,25044 |
| 010_s_9 | 10 | 29.610 | 0,03637 | 0,17019 |
| 020_sm_0 | 20 | 48.693 | 0,21610 | 1,78815 |
| 020_sm_1 | 20 | 53.658 | 0,23730 | 1,76421 |
| 020_sm_2 | 20 | 45.384 | 0,10709 | 0,64581 |
| 020_sm_3 | 20 | 49.323 | 0,41923 | 3,87906 |
| 020_sm_4 | 20 | 44.677 | 0,25804 | 2,02248 |
| 020_sm_5 | 20 | 42.289 | 0,20588 | 1,41190 |
| 020_sm_6 | 20 | 51.353 | 0,24259 | 1,78205 |
| 020_sm_7 | 20 | 45.288 | 0,29124 | 2,49672 |
| 020_sm_8 | 20 | 45.257 | 0,23576 | 1,97495 |
| 020_sm_9 | 20 | 48.117 | 0,16028 | 0,93129 |
| 040_m_0 | 40 | 92.802 | 2,82971 | 26,82781 |
| 040_m_1 | 40 | 91.263 | 1,84738 | 15,95464 |
| 040_m_2 | 40 | 90.792 | 0,71296 | 4,77066 |
| 040_m_3 | 40 | 91.971 | 2,10037 | 20,16199 |
| 040_m_4 | 40 | 94.764 | 2,56792 | 21,79439 |
| 040_m_5 | 40 | 94.143 | 1,16249 | 8,75681 |
| 040_m_6 | 40 | 91.063 | 2,38876 | 21,92190 |
| 040_m_7 | 40 | 101.150 | 3,64844 | 35,97391 |
| 040_m_8 | 40 | 81.903 | 0,87161 | 7,01236 |
| 040_m_9 | 40 | 92.821 | 3,12038 | 29,80253 |
| 080_ml_0 | 80 | 192.792 | 16,90118 | 108,46413 |
| 080_ml_1 | 80 | 192.077 | 63,48429 | 561,80182 |
| 080_ml_2 | 80 | 187.872 | 27,11544 | 187,47461 |
| 080_ml_3 | 80 | 187.696 | 20,61190 | 129,28365 |
| 080_ml_4 | 80 | 184.996 | 34,35254 | 305,59034 |
| 080_ml_5 | 80 | 190.548 | 36,67534 | 326,07649 |
| 080_ml_6 | 80 | 188.204 | 31,12883 | 256,97143 |
| 080_ml_7 | 80 | 193.148 | 86,12654 | 831,54008 |
| 080_ml_8 | 80 | 189.504 | 25,41119 | 167,63328 |
| 080_ml_9 | 80 | 190.652 | 25,52500 | 211,01538 |
| 100_l_0 | 100 | 240.460 | 292,45697 | 2.880,82138 |
| 100_l_1 | 100 | 236.104 | 168,22690 | 1.554,66110 |
| 100_l_2 | 100 | 231.468 | 186,77969 | 1.658,41696 |
| 100_l_3 | 100 | 234.192 | 68,61825 | 432,92631 |
| 100_l_4 | 100 | 232.436 | 97,99636 | 898,65973 |
| 100_l_5 | 100 | 229.836 | 171,34566 | 1.568,17337 |
| 100_l_6 | 100 | 230.344 | 314,79868 | 3.189,95584 |
| 100_l_7 | 100 | 232.536 | 68,79076 | 586,30417 |
| 100_l_8 | 100 | 233.044 | 184,59165 | 1.660,02422 |
| 100_l_9 | 100 | 234.676 | 51,99832 | 415,53863 |

: Run-time  and results of the instances tested. The table displays the weight of the TSP tour, the user time
and the CPU time, in seconds. \label{tab:results}

| Nodes | User time (s) | CPU time (s) | Std dev of CPU time (%) |
| :--- | ---: | ---: | ---: |
| 10 | 0,04116 | 0,18576 | 36,14% |
| 20 | 0,23735 | 1,86966 | 47,43% | 
| 40 | 2,12500 | 19,29770 | 53,05% |
| 80 | 36,73322 | 308,58512 | 73,00% |
| 100 | 160,56032 | 1484,54817 | 64,80% |

: Run-time of the instances tested, grouped by size. For each size the average of user time, and the CPU time is displayed, in seconds. The table displays the standard deviation of the CPU time. \label{tab:resultsAverage}

Table \ref{tab:resultsAverage} shows the average CPU and user time per instance size: we can now answer the initial question. Note that, by comparing tables \ref{tab:results} with the latter,
completion times between instances with the same size differ, sometimes by a considerable amount:
take for example `100_l_6` and `100_l_9`. The answer, with respect to the user time:

 - the MILP implementation is definitely able to solve a 10 nodes instance in under 0.1 seconds
 - only some of the 40 nodes instances can be solved in less than a second, and comfortably under 10 seconds
 - doubling the size of the instance more than doubles the amount of time thus, an instance of size of 80 nodes can be solved in less than 100 seconds.

Note that there might be instances of size 40 that take more than 10 seconds to solve and size 80 that take more than 100 seconds to solve, that is because the CPLEX solver explores the solution space differently for each instance.

It is interesting to notice that the CPU time starts with (roughly) a 1 to 5 ratio, with respect to the user time, and ends with a 1 to 9 ratio: multi-threading becomes more and more an important the bigger the instances are.

![A logarithmic scale graph that shows user time and CPU time of obtaining a solution for TSP, plotted against the number of nodes of the instances.\label{img:timePlotLog}](src/cpuAndUserTimeLog.pdf)

![A graph that shows user time and CPU time of obtaining a solution for TSP, plotted against the number of nodes of the instances.\label{img:timePlot}](src/cpuAndUserTime.pdf)

\ref{img:timePlotLog} is a scatter plot, with a trend line, calculated via LOESS (LOcally Estimated Scatterplot Smoothing) which approximates the values for each x coordinate. Essentially, the y coordinate is the average of the CPU and user time obtained. The plot is in logarithmic scale, which allows me to see the exponential nature of a MILP approach, the curve slows down in its growth towards the end, thus I cannot definitely say that the complexity of this MILP implementation is exponential. It is also interesting to notice that the trend line for the user time show the same behavior, but with a slightly less steep growth, which can be better visualized in the linear scale version of the graph, \ref{img:timePlot}.

# Conclusion

In this brief report I showed a in implementation of a MILP model for the TSP, such implementation finds an optimal answer in exponential time complexity, or a very high degree polynomial time complexity. 
The implementation uses CPLEX, which is among the best commercial linear programming solvers. Execution times rise quickly until it becomes unfeasible to use a solver to obtain an exact result. The solution is to use an ad-hoc solver, like Concorde [@noauthor_concorde_nodate], which provides faster execution times, or a heuristic, which provides even faster execution times, but solutions that are not necessarily exact.


# References
