---
title:  'Models and Methods for Combinatorial Optimization: Assignment 2'
author:
- Alessandro Flori^[Department of Mathematics ``Tullio Levi-Civita'', University of Padua]
institute: 
- Department of Mathematics ``Tullio Levi-Civita''
- University of Padua
date: 17 February, 2023
abstract: |
  The goal of this assignment is to solve
  the Traveling Salesperson Problem (TSP) via either an exact approach or an heuristic,
  and to compare its performance with a Mixed Integer Linear Programming implementation.
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

In this report I show the implementation of a heuristic for the Traveling Salesperson Problem (TSP).
The heuristic is the Lin-Kernighan (LK) heuristic for TSP [@lin_effective_1973].
LK is a very successful attempt at solving TSP: it is able to solve instances with, as of 1971, when Lin and Kernighan wrote the paper, unknown optimal solutions, with near optimal results. And it was able to do so in a reasonable estimated complexity of $O(n^{2.2})$.
The Lin-Kernighan heuristic is the starting point of a more advanced heuristic called Lin-Kernighan-Helsgaun (LKH). LKH is currently one of the most performant algorithms for TSP [@HELSGAUN2000106].

In the first sections of the paper I show the LK implementation. Then I compare LK with a MILP model for TSP. The comparison shows the difference in computation time, optimality of the solutions. 

# Lin-Kernighan heuristic
 
## Implementation

# Tests

## Test data

## Test results

# Conclusion

# References
