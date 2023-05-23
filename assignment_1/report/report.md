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
header-includes: |
  \lstdefinestyle{tree}{
    literate=
    {├}{{\smash{\raisebox{-1ex}{\rule{1pt}{\baselineskip}}}\raisebox{0.5ex}{\rule{1ex}{1pt}}}}1 
    {─}{{\raisebox{0.5ex}{\rule{1.5ex}{1pt}}}}1 
    {└}{{\smash{\raisebox{0.5ex}{\rule{1pt}{\dimexpr\baselineskip-1.5ex}}}\raisebox{0.5ex}{\rule{1ex}{1pt}}}}1 
  }
...

# Introduction

A company produces boards with holes used to build electric panels. 
The company wants to minimize the total drilling time for each board, taking into account that the time needed for making an hole is the same and constant for
all the holes. This is an instance of the Traveling Salesperson Problem (TSP).

In this report I show an implementation of a (mixed) integer linear programming model (MILP)
of the TSP. The implementation is written in the 
C++ language and uses the CPLEX Callable Library [@noauthor_ibm_2021]. The goal is to 
study up to which size the problem can be solved in different amounts of time: 0.1 second, 1 second, 10 seconds. 

Follows a description of the model and its implementation.
Section [Tests] contains a description of the instances used and a study of the model's behavior under different sizes of problems.

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

# Implementation

## TSP instance for the drilling problem

An ad-hoc solution was created to generate realistic instances for the drilling problem.

Instances are produced via a script, which is located in the folder `instance_generator` in the root of the project.
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

## The model implementation

Files `model.h` and `model.cpp` contain the model interface and implementation, respectively. I tried to keep the implementation as close as possible to the MILP model described in [Introduction].

> Note that every `NULL` field is automatically filled by a default value by the CPLEX API.

```{#lst:initYVars .c caption="Initialization of variable (7), from the MILP model."}
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < N; ++j) {
			char ytype = 'B';
			double lb = 0.0;
			double ub = 1.0;
      double coef = i == j ? CPX_INFBOUND : c[i][j];
			CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &coef, &lb, &ub, &ytype, NULL);
    }
  }
```

```{#lst:initXVars .c caption={Initialization of variable (6), from the MILP model.}}
  for(int i = 0; i < N * (N - 1); ++i) {
      CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, NULL, NULL, NULL, NULL, NULL);
  }
```

Listings \ref{lst:initYVars} and \ref{lst:initXVars} contain the initialization of the variables, (6) and (7) from the model, respectively.
Note that:

 - each $y_{i,j}$ variable is coupled with its weight, $c_{i,j}$, the weight $c_{i,i}$ is replaced by an infinite value, that is because otherwise the model considers the edge $(i,i)$ while building the hamiltonian cycle ($c_{i,i} = 0$, since the distance between node $i$ and node $i$ is $0$)
 - constraint (6) describes $x_{i,j}$ as a positive, real, number coupled with each edge in $A$, except for $(i,j), j = 0$, thus I initialized $N * (N - 1)$ variables, which are variables $x_{0,1}, x_{0,2}, ..., x_{1,1},..., x_{|N| - 1, |N| - 1}$
 - the default values for type and bounds of a variable create a continuous number, bounded between zero and infinite.

```{#lst:firstConstraint .c caption="Creation of the first constraint, it's (2) in the model definition in the introduction."}
  for(int k = 0; k < N - 1; ++k) {
    std::vector<int> idx;
    std::vector<double> coef;
    for(int i = 0; i < N; ++i) { // x_i_k
      if(k + 1 != i) {
        idx.push_back(N * N + i * (N - 1) + k);
        coef.push_back(1.0);
      }
    }
    for(int j = 0; j < N - 1; ++j) { // x_k_j
      if(k != j) {
        idx.push_back(N * N + (k + 1) * (N - 1) + j);
        coef.push_back(-1.0);
      }
    }
    double rhs = 1.0;
    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL);
  }
```

```{#lst:secondConstraint .c caption="Constraint (3) from the model definition in the introduction."}
  for(int i = 0; i < N; ++i) {
    std::vector<int> idx(N);
    for(int j = 0; j < N; ++j) { // y_i_j 
      idx[j] = i * N + j;
    }
    std::vector<double> coef(N, 1.0);
    double rhs = 1.0;
    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL);
  }
```

```{#lst:thirdConstraint .c caption="Constraint (4) from the model definition in the introduction."}
  for(int j = 0; j < N; ++j) {
    std::vector<int> idx(N);
    for(int i = 0; i < N; ++i) { // y_i_j 
      idx[i] = i * N + j;
    }
    std::vector<double> coef(N, 1.0);
    double rhs = 1.0;
    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL);
  }
```

```{#lst:fourthConstraint .c caption="Constraint (5) from the model definition in the introduction."}
  for (int i = 0; i < N; ++i) {
		for (int j = 1; j < N; ++j) {
			std::vector<int> idx(2);
      idx[0] = i * N + j; // y_i_j
			idx[1] = N * N + i * (N - 1) + j - 1; // x_i_j
			std::vector<double> coef(2, 1.0);
			coef[0] = - N + 1.0;
			char sense = 'L';
			int matbeg = 0;
			CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), NULL, &sense, &matbeg, &idx[0], &coef[0], NULL, NULL);
		}
	}
```

# Tests

## Test data

Through the instance generator I generated 5 different types of instances:

 - 10 nodes, size of two-dimensional plane $10\times 10$cm, the distance between nodes in the grid is $1,5$cm
 - 20 nodes, $10\times 15$cm, $1,5$cm
 - 40 nodes, $20\times 15$cm, $1,5$cm
 - 80 nodes, $30\times 20$cm, $2,0$cm
 - 100 nodes, $30\times 25$cm, $2,0$cm.

There are 10 randomly generated instances of each size.

<!-- TODO: new test results -->

## Test results

> Tests were executed on a linux laptop, with 8gb of ram and a 6 cores, 12 threads CPU, with a local CPLEX install.

Table \ref{tab:results} shows user and CPU time of the different instances.
The user time is the time the test took to complete.
The CPU time is the time the test took to complete, taking into account all cores time of use:
CPLEX uses multi-threading to solve linear programming problems, CPU time can be thought as
the total time it could take if the test was run on a single-core CPU.

| Instance | Nodes | Result | User time | CPU time |
| --- | ---: | ---: | ---: | ---: |
| 010_s_0 | 10 | 3.080.494 | 0,04262 | 0,09443 |
| 010_s_1 | 10 | 3.097.542 | 0,04180 | 0,20734 |
| 010_s_2 | 10 | 2.473.785 | 0,03837 | 0,19138 |
| 010_s_3 | 10 | 2.109.674 | 0,03757 | 0,16849 |
| 010_s_4 | 10 | 2.671.884 | 0,03877 | 0,20148 |
| 010_s_5 | 10 | 2.604.758 | 0,02972 | 0,16866 |
| 010_s_6 | 10 | 2.382.952 | 0,04682 | 0,18905 |
| 010_s_7 | 10 | 2.541.640 | 0,00633 | 0,00876 |
| 010_s_8 | 10 | 2.648.528 | 0,01785 | 0,04115 |
| 010_s_9 | 10 | 2.648.127 | 0,02371 | 0,05907 |
| 020_sm_0 | 20 | 4.378.036 | 0,10557 | 0,58530 |
| 020_sm_1 | 20 | 4.724.210 | 0,34074 | 2,77973 |
| 020_sm_2 | 20 | 4.480.164 | 0,13944 | 0,59811 |
| 020_sm_3 | 20 | 4.731.912 | 0,31655 | 2,69098 |
| 020_sm_4 | 20 | 4.414.432 | 0,26387 | 1,16535 |
| 020_sm_5 | 20 | 4.537.967 | 0,33559 | 2,47486 |
| 020_sm_6 | 20 | 5.005.086 | 0,19891 | 1,14426 |
| 020_sm_7 | 20 | 4.403.364 | 0,21031 | 1,39348 |
| 020_sm_8 | 20 | 5.129.022 | 0,20553 | 1,03018 |
| 020_sm_9 | 20 | 4.579.100 | 0,15504 | 0,94832 |
| 040_m_0 | 40 | 9.448.877 | 2,65303 | 24,42623 |
| 040_m_1 | 40 | 9.400.019 | 1,37242 | 11,54096 |
| 040_m_2 | 40 | 9.427.142 | 4,21307 | 39,87490 |
| 040_m_3 | 40 | 10.055.586 | 2,81334 | 26,98918 |
| 040_m_4 | 40 | 9.364.024 | 2,62407 | 24,26424 |
| 040_m_5 | 40 | 9.559.108 | 5,76872 | 58,45387 |
| 040_m_6 | 40 | 8.899.356 | 1,13352 | 9,29531 |
| 040_m_7 | 40 | 9.003.871 | 1,79128 | 16,91499 |
| 040_m_8 | 40 | 9.482.308 | 2,50331 | 24,25281 |
| 040_m_9 | 40 | 9.586.823 | 1,61283 | 13,96239 |
| 080_ml_0 | 80 | 18.906.712 | 19,85417 | 119,87202 |
| 080_ml_1 | 80 | 18.716.974 | 57,09734 | 507,57514 |
| 080_ml_2 | 80 | 19.019.916 | 7,36798 | 40,21931 |
| 080_ml_3 | 80 | 19.361.558 | 68,23351 | 654,73879 |
| 080_ml_4 | 80 | 18.500.129 | 33,11392 | 228,61491 |
| 080_ml_5 | 80 | 19.397.187 | 17,79925 | 118,00863 |
| 080_ml_6 | 80 | 19.504.073 | 25,35637 | 205,84769 |
| 080_ml_7 | 80 | 18.854.230 | 17,07445 | 101,67009 |
| 080_ml_8 | 80 | 19.195.872 | 84,90501 | 793,00571 |
| 080_ml_9 | 80 | 18.385.601 | 28,34536 | 199,10756 |
| 100_l_0 | 100 | 23.377.088 | 180,34132 | 1.532,21582 |
| 100_l_1 | 100 | 23.565.503 | 128,62939 | 1.059,22186 |
| 100_l_2 | 100 | 23.399.817 | 310,69360 | 3.097,85009 |
| 100_l_3 | 100 | 23.333.900 | 235,10626 | 2.074,80375 |
| 100_l_4 | 100 | 22.902.759 | 63,22106 | 467,02487 |
| 100_l_5 | 100 | 23.496.875 | 188,79845 | 1.829,13587 |
| 100_l_6 | 100 | 23.351.288 | 292,66543 | 2.897,26336 |
| 100_l_7 | 100 | 23.318.289 | 127,69253 | 1.075,14772 |
| 100_l_8 | 100 | 23.812.717 | 262,98502 | 2.531,96917 |
| 100_l_9 | 100 | 23.598.502 | 212,14048 | 1.944,59297 |

: Run-time  and results of the instances tested. The table displays the weight of the TSP tour, the user time
and the CPU time, in seconds. \label{tab:results}

| Nodes | UserTime | CPU time |
| --- | --- | --- |
| 10 | 0,0324 | 0,1330 |
| 20 | 0,2272 | 1,4811 |
| 40 | 2,6486 | 0,5479 |
| 80 | 35,9147 | 296,8660 |
| 100 | 200,2274 | 1850,9225 |

: Run-time of the instances tested, grouped by size. For each size the average of user time, and the CPU time is displayed, in seconds. User time and CPU time were rounded to the 4th decimal. \label{tab:resultsAverage}

Table \ref{tab:resultsAverage} shows the average CPU and user time per instance size: we can now answer the initial question. Note that, by comparing tables \ref{tab:results} with the latter,
completion times between instances with the same size differ, sometimes by a lot:
take for example xqf060 and pma060. The answer, with respect to the user time:

 - the MILP implementation is definitely able to solve a 10 nodes instance in under 0.1 seconds,
 and some of the 20 nodes implementations as well
 - only some of the 40 nodes instances can be solved in less than a second
 - only some of the 60 nodes instances can be solved in less than 10 seconds.

It is interesting to notice that the CPU time starts with (roughly) a 1 to 5 ratio, the user time is 5 times smaller than the CPU time, and ends with a 1 to 9 ratio: multi-threading becomes more and more an important the bigger the instances are.
When considering the CPU time, clearly none of the 10 nodes instances can be solved in less than .1 seconds, but all of them, along with some with a size of 20, can be solved within less than a second. Only some instances with a size of 40 can be solved in less than 10 seconds.

<!-- graph 1 shows the exponential nature of MILP (with respect to time complexity.)  -->

# Conclusion

In this brief report I showed a in implementation of a MILP model for the TSP, such implementation finds an optimal answer in exponential time complexity, or a very 
high degree polynomial time complexity, it depends on the CPLEX implementation and the optimizations it applies. 


# References
