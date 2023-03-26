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

## Issues of implementation

1. The set data structure has a high computational complexity in lookup, insertion, deletion of entries, but it greatly simplifies the development process: it allows to easily translate the pseudo-code into the actual C++ implementation. Real world implementations use data structures such as linked lists or arrays of nodes.

2. The implementation is a recursive algorithm: while it has a limit on the recursion depth, inherited by LK itself, some key requirements described [@lin_effective_1973] are missing <!-- such as ... -->. These missing requirements do not harm the optimality of the solution, but they do harm the computational complexity of the algorithm.

3. I did not take into account space complexity: when [@lin_effective_1973] was published it was a problem, and instances with sizes above 100 were a problem. As of now, storage utilization is certainly not the first concern. A possible solution, that would reduce storage utilization, while not increasing the complexity of the implementation, is the use of persistent data structures [@driscoll-1986]. Persistent data structures would allow having multiple versions of the same data structure, without having copies of such data structures. Many programming languages offer persistent data structures in their standard library, such as Scala. 
C++ only offers persistent data structures through third party libraries.

# Tests

## Test data

## Test results

Instance	 Min cost  Max cost  Mean cost  Std dev cost  Min CPU time  Max CPU time  Mean CPU time  Std dev CPU time
--------- --------- --------- ---------- ------------- ------------- ------------- -------------- -----------------
pka010         30       30           30        0.0000        0.0011        0.0018         0.0014            0.0002
pka020         59       59           59        0.0000        0.0024        0.0067         0.0044            0.0013
pka040        152      158	        155        3.0000        0.0988        0.2791         0.1733            0.0564
pka060        224      231	        226        2.3238        0.2834        0.5526         0.4136            0.0874
pka120        467      478	        471        3.7417        8.0852       14.0954        10.6979            2.0105
pma010         66       66           66        0.0000        0.0011        0.0015         0.0013            0.0001
pma020         72       72           72        0.0000        0.0106        0.0176         0.0139            0.0022
pma040        153      154	        153        0.3162        0.0446        0.1103         0.0733            0.0208
pma060        228      237	        231        2.6833        0.2975        0.5371         0.3964            0.0625
pma120        476      483	        478        2.7929        5.3666       12.0066         8.5481            1.7753
xqf010         81       81           81        0.0000        0.0013        0.0029         0.0022            0.0005
xqf020        116      116	        116        0.0000        0.0066        0.0144         0.0098            0.0025
xqf040        158      159	        158        0.4472        0.0208        0.0759         0.0313            0.0167
xqf060        224      228	        225        1.5811        0.2040        0.4447         0.2899            0.0735
xqf120        483      493	        489        2.9326        4.7206       16.0637         8.7806            3.0704
xqg010         97       97           97        0.0000        0.0013        0.0016         0.0014            0.0001
xqg020        132      132	        132	       0.0000        0.0105        0.0241         0.0159            0.0048
xqg040        193      194	        193	       0.3162        0.1107        0.1929         0.1410            0.0222
xqg060        255      263	        258	       2.4900        0.1590        0.3252         0.2376            0.0512
xqg120        557      576	        564	       6.9714        4.6334       13.8637         9.3266            2.5203

: Run-time  and results of the instances tested, using the Lin-Kernighan heuristic implementation. The table displays the weight of the TSP tour, the user time
and the CPU time, in seconds. \label{tab:results}


# Conclusion

# References
