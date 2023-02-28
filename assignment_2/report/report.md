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

Instance,Nodes,Edges,Result,User time,CPU time
pka010,10,100,30,0.00244498,0.002447
pka020,20,400,60.0061,0.0122151,0.012175
pka040,40,1600,152.087,1.32314,1.32123
pka060,60,3600,225,3.53624,3.53195
pka120,120,14400,472.284,154.186,153.981
pma010,10,100,66,0.00215387,0.002148
pma020,20,400,72.4775,0.0483072,0.048194
pma040,40,1600,155.649,0.466176,0.465509
pma060,60,3600,237.082,4.32367,4.31767
pma120,120,14400,488.222,108.883,108.722
xqf010,10,100,81.8798,0.00622201,0.00619
xqf020,20,400,115.498,0.057502,0.057401
xqf040,40,1600,157.512,0.11927,0.119111
xqf060,60,3600,225.09,2.80433,2.80067
xqf120,120,14400,486.651,100.48,100.335
xqg010,10,100,98.6212,0.0048511,0.00484
xqg020,20,400,133.606,0.027863,0.027813
xqg040,40,1600,194.575,0.550078,0.549321
xqg060,60,3600,257.833,1.79939,1.79699
xqg120,120,14400,586.468,103.908,103.761

# Conclusion

# References
