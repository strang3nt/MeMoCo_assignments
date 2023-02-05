---
title:  'Models and Methods for Combinatorial Optimization: Assignment 1'
author:
- Alessandro Flori^[Department of Mathematics "Tullio Levi-Civita, University of Padua]
institute: 
- 'Department of Mathematics "Tullio Levi-Civita"'
- "University of Padua"
date: 17 February, 2023
abstract: |
  The goal of this assignment is to solve
  the Traveling Salesman Problem via Mixed Integer Linear Programming, and
  study up to which size the problem can be solved in different amounts of time (e.g. 0.1s, 1s, 10s,...). The implementation uses the CPLEX API.
geometry: margin=1in
lang: en-GB
bibliography: references.bibtex
colorlinks: yes
...

# Introduction

In this report I show an implementation of a (mixed) integer linear programming model 
of the Traveling Salesperson Problem (TSP). The implementation is written in the 
C++ language and uses the CPLEX Callable Library [@noauthor_ibm_2021].

The model is exactly the one presented in [@gavish_travelling_1978].

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

Follows a description of the implementation.
Section [Tests] contains a description of the instances used and a study of the model's behavior under different sizes of problems: in particular 

# Implementation

 > Every `NULL` field is automatically filled by a default value by the CPLEX API.

# Tests

## Test data

I retrieved 4 test instances from [@noauthor_vlsi_nodate]. This website
contains many instances of TSP. Each file contains a number of node coordinates, and the weight of an edge $(i,j)$ is the euclidean distance between node $i$ and $j$, in a 2d plane.

From each instance I built a 10, 20, 40, 60, and a 120 nodes instance: the goal was to have instances which could be solved by my machine in (around) 0.1 second, 1 second and under 10 seconds, and to have two more data points in order to plot a more meaningful graph.

## Test results

> The tests were run on a linux laptop, with 8gb of ram and a 6 cores, 12 threads CPU, with a local CPLEX install.

# Conclusion

```{#some_code .c caption="Some code."}
// add y vars
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
# References
