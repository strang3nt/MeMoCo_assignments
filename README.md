## Mthods and Models for Combinatorial Optimization (MeMoCo) assignments

This repository contains solutions for the 2 assignments of the MeMoCo
course from the Master's degree in Computer Science at Unipd.

The first assignment uses [IBM ILOG CPLEX](https://www.ibm.com/analytics/cplex-optimizer) C apis, to solve a set of TSP instances via linear programming.
The second assignment uses a heuristic to solve the same set of instances. 

Test cases were retrieved from the [VLSI Data Sets](https://www.math.uwaterloo.ca/tsp/vlsi/index.html). The dataset files were modified in order to have different sizes and to be parsed in an easier manner.

## First assignment

The goal of this assignment is to solve
the Traveling Salesman Problem via Mixed Integer Linear Programming, and
study up to which size the problem can be solved in different amounts of time (e.g. 0.1s, 1s, 10s,...).

The implementation uses CPLEX APIs.
