## Methods and Models for Combinatorial Optimization (MeMoCo) assignments

This repository contains solutions for the 2 assignments of the MeMoCo
course from the Master's degree in Computer Science at Unipd.

The first assignment uses [IBM ILOG CPLEX](https://www.ibm.com/analytics/cplex-optimizer) C APIs, it solves a set of TSP instances via linear programming.
The second assignment uses a heuristic to solve the same set of instances. 

## First assignment

The goal of this assignment is to solve
the Traveling Salesman Problem via Mixed Integer Linear Programming, and
study up to which size the problem can be solved in different amounts of time (e.g. 0.1s, 1s, 10s,...).

## Second assignment

In this assignment I implement the Lik-Kernighan heuristic for 
TSP. The implementation follows the pseudo-code from 
the following Wikipedia article: <https://en.wikipedia.org/wiki/Lin%E2%80%93Kernighan_heuristic?oldformat=true>.
In the report I show the performance of the heuristic, in
terms of both time and optimality of the solution.

## References

 - <https://en.wikipedia.org/wiki/Lin%E2%80%93Kernighan_heuristic?oldformat=true>
 - S. Lin, B. W. Kernighan, (1973) An Effective Heuristic Algorithm for the Traveling-Salesman Problem. Operations Research 21(2):498-516.
<https://doi.org/10.1287/opre.21.2.498>
