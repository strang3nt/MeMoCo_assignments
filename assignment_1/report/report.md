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
  the Traveling Salesperson Problem (TSP) via a Mixed Integer Linear Programming model, and
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

In this report I show an implementation of a (mixed) integer linear programming model (MILP)
of the Traveling Salesperson Problem (TSP). The implementation is written in the 
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

Listing \ref{lst:projectStructure} contains the structure of the project.

\begin{lstlisting}[caption={Structure of project folder.},label={lst:projectStructure},style=tree]
.
├── Makefile
└── src
    ├── cpxmacro.h
    ├── graph.h
    ├── main.cpp
    ├── model.cpp
    ├── model.h
    ├── parser.cpp
    └── parser.h
\end{lstlisting}

cpxmacro

: Contains useful macros for the CPLEX API. Environment and problem initialization macros, as well as a macro to make a checked call to the CPLEX API. Every call to the CPLEX API goes first through here.

graph

: Contains a class that represents an instance of a graph. Each graph contains its size (number of nodes) and a matrix of size $N \times N$, where $N$ is the size. Cell $(i, j)$ of the matrix contains the distance from node $i$ to node $j$. 

parser

: It is the definition of a class that contains a method `buildGraph`, such
method, given a file path tries to parse such file. 
The file must have the 
following structure: starts with the number of nodes, and each new line contains the name of the node, the $x$ and $y$ coordinates. The output of `buildGraph` is a `Graph` instance, with the weights as the euclidean distance between the nodes. Follows an example of a file.

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

model

: The implementation of MILP model.

main

: Puts everything together: takes the address of a file as input, such file is parsed as a graph, the model is run against the graph, prints user time and CPU time taken to solve the model instance.


## The model implementation

Files `model.h` and `model.cpp` contains model interface and implementation, respectively. I tried to keep the implementation as close as possible to the MILP model described in [Introduction].

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
 - constraint (6) describes $x_{i,j}$ as a positive, real, number coupled with each edge in $A$, except for $(i,j), j = 0$, thus I initialized $N * (N - 1)$ variables, which are variables $x_{0,1}, x_{0,2}, ..., x_{1,1},..., x_{|N| - 1, |N| - 1}$. 

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

I retrieved 4 test instances from [@noauthor_vlsi_nodate]. This website
contains many instances of TSP. Each file contains a number of node coordinates, and the weight of an edge $(i,j)$ is the euclidean distance between node $i$ and $j$, in a 2d plane.

Each initial instance was cut down to a 10, 20, 40, 60, and a 120 nodes: the goal was to have instances which could be solved by my machine in (around) 0.1 second, 1 second and under 10 seconds, and to have two more data points in order have more meaningful data.

## Test results

> The tests were executed on a linux laptop, with 8gb of ram and a 6 cores, 12 threads CPU, with a local CPLEX install.

Table \ref{tab:results} shows user and CPU time of the different instances.
The user time is the time the test took to complete.
The CPU time is the time the test took to complete, taking into account all cores time of use:
CPLEX uses multi-threading to solve linear programming problems, CPU time can be thought as
the total time it could take if the test was run on a single-core CPU.

  Instance   Result  User time(seconds)  CPU time(seconds)
  --------- ------- ------------------- ------------------
   pka010       30              0.0812        0.3117
   pka020       59              0.1437        0.4020
   pka040      152              1.8568        17.3273
   pka060      224              1.4288        7.5849
   pka120      466            313.7200        3.096.3200
   pma010       66              0.0643        0.3784
   pma020       72              0.2844        2.4847
   pma040      153              0.8433        6.8137
   pma060      228              4.5828        33.0678
   pma120      474            208.2600        1.889.1000
   xqf010       81              0.0754        0.3308
   xqf020      116              0.2673        2.1453
   xqf040      158              0.7773        5.1893
   xqf060      224             13.7678        120.2260
   xqf120      480            253.4320        2.459.2800
   xqg010       97              0.1395        0.6386
   xqg020      132              0.4963        4.5762
   xqg040      193              2.6379        25.2748
   xqg060      255              3.8536        27.3121
   xqg120      554            267.3830        2.619.4800

: Run-time  and results of the instances tested. The table displays the weight of the TSP tour, the user time
and the CPU time, in seconds. \label{tab:results}

 Nodes	User time    CPU time
------ ---------- -----------
10        0.0901      0.4149
20        0.2979      2.4020
40        1.5288     13.6513
60        5.9082     47.0477
120     260.6988   2516.0450

: Run-time of the instances tested, grouped by size. For each size the average of user time, and the CPU time is displayed, in seconds. User time and CPU time were rounded to the 4th decimal. \label{tab:resultsAverage}

Table \ref{tab:resultsAverage} shows the average CPU and user time per instance size: we can now answer the initial question. Note that, by comparing tables \ref{tab:results} with the latter,
completion times between instances with the same size differ, sometimes by a lot:
take for example xqf060 and pma060. The answer, with respect to the user time:

 - the MILP implementation is definitely able to solve a 10 nodes instance in under 0.1 seconds,
 and some of the 20 nodes implementations as well
 - only some of the 40 nodes instances can be solved in less than a second
 - only some of the 60 nodes instances can be solved in less than 10 seconds.

It is interesting to notice that the CPU time starts with (roughly) a 1 to 5 ratio, the user time is 5 times smaller than the CPU time, and ends with a 1 to 9 ratio: multi-threading becomes more and more an important the bigger the instances are.
By considering the CPU time, I see that none of the 10 nodes instances can be solved in less than .1 seconds, but all of them, along with some with a size of 20, can be solved within less than a second. Only some instances with a size of 40 can be solved in less than 10 seconds.

<!-- graph 1 shows the exponential nature of MILP (with respect to time complexity.)  -->


# Conclusion

In this brief report I showed a in implementation of a MILP model for the TSP, such implementation finds an optimal answer in exponential time complexity, or a very 
high degree polynomial time complexity, it depends on the CPLEX implementation and the optimizations it applies. 


# References
