---
title:  'Models and Methods for Combinatorial Optimization: Assignment 2'
author:
- Alessandro Flori^[Department of Mathematics ``Tullio Levi-Civita'', University of Padua]
institute: 
- Department of Mathematics ``Tullio Levi-Civita''
- University of Padua
date: 17 February, 2023
abstract: |
  The goal of this assignment is to solve a drilling problem using the Traveling Salesperson Problem (TSP) via either an exact approach or an heuristic,
  and to compare its performance against an implementation of a Mixed Integer Linear Programming model for TSP.
geometry: margin=1in
lang: en-GB
bibliography: references.bibtex
colorlinks: yes
header-includes: |

  \usepackage[ruled]{algorithm2e}
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

In this report I show the implementation of a heuristic for the Traveling Salesperson Problem (TSP).
The heuristic is the Lin-Kernighan (LK) heuristic for TSP [@lin_effective_1973].
LK is a very successful attempt at solving TSP: it was able to solve instances with, at the time (1971), unknown optimal solutions, with near optimal results. And it was able to do so within a $O(n^{2.2})$ time complexity.
The Lin-Kernighan heuristic is the starting point for a more advanced heuristic called Lin-Kernighan-Helsgaun. The latter is, to this day, one of the best performing algorithms for TSP [@HELSGAUN2000106].

In the first section of the paper I show the LK implementation. Then I compare LK with a MILP model for TSP. The comparison shows the difference in computation time, and optimality of the solutions. 

# Lin-Kernighan heuristic

## Graph-theretical background

Let $G = (V, E)$ be an undirected, complete graph, where $V$ is the set of nodes, and $E$ is the set of edges where $(u, v) \in E$ is an edge.
Let $c: E \to\mathbb{Z}$ be a total function that given an edge $(u, v)$ returns its cost.
A tour is an hamiltonian cycle, a path in the graph which visits all nodes exactly once, except the starting node, which is visited twice.
Given a tour $T$ an alternating trail is a list of edges such that they are alternatingly in $T$ and not in $T$. An edge $(u, v)$ is said to be adjacent to a particular node if that node is either $u$ or $v$, any number of edges is said to be adjacent if they share one common node.

I will use operations from set theory such as union, $\cup$, set difference $\\$ and symmetric difference, $\Delta$.

## Implementation

Lin and Kernighan developed their algorithm in a time when branch statements (or goto) were widely used. The resulting algorithm is very complicated to understand. In the following paragraphs I try to show an iterative version of LK, as well as a recursive version.

The goal of the algorithm is to build the best possible tour by making local changes to a starting tour.
In the algorithm I use the following notation:
 
 - $T$ is a list of edges, and it denotes a tour of $G$
 - a tour $T$ of size $n$ is ordered in the sense that $T[0] = x_0 = (t_0, t_1)$ is the first edge of the trail, $T[1] = x_1 = (t_1, t_2)$, $T[i] = x_i = t_{2i - 1}, t_{2i}$
 - $x_i$ is an edge I want to remove from a tour, I want to exchange such edge with $y_i$, it is always the case that $x_i\in T$ and $y_i \notin T$
 - let $g_i = c(x_i) - c(y_i)$ be the gain from exchanging $x_i$ from a tour $T$ with $y_i$.
 - all $x_i$ and $y_i$ form two sets, respectively $X$ and $Y$, with these 2 sets I can compute $G_i$ which is the sum of all the improvements of exchanging the edges, up to the $i$-th. A possible implementation of LK, could build $X$ and $Y$ as arrays of edges, that are modified multiple times during the execution of the algorithm.

Many implementation details are missing, still, this should be a good starting point, and a good outline of LK.

1. For all $t \in T$, let $t$ be $t_1$ do:
  1. Set $G^* = 0$. For all edges adjacent to $t_1$ such that they are in the tour, let one such edge be $x_1$ and let $t_2$ be the other node. Set $i = 1$. For all $x_1$, until $G^* = 0$, do:
    1. Compute all possible $y_1$, such that $y_1 = (t_2, t_3)$, and $y_1\notin T$, ordered by improvement $g_1 > 0$, descending. For all $y_1$ do:
      1. Let $i = 2$, and compute all possible $x_2$ such that if it is joined to the first node of the tour, the resulting configuration is a tour, until $G^* = 0$ do:
        1. Compute all $y_2$, order of increasing cost $c(y_2)$, following conditions (a), (b), (c), which are described below:
          1. Repeat until it is not possible to build $x_i$, $y_i$
              1. Let $i = i + 1$, and choose $x_i$, which currently joins $t_{2i - 1}$ to $t_{2i}$, and $y_i$ as follows:
              2. let $x_i$ be such that if $t_{2i}$ is joined to $t_1$, the resulting configuration is a tour
              3. let $y_i$ be some available link available at the node $t_{2i}$ shared with $x_i$, subject to (a), (b), (c)
              4. before $y_i$ is constructed, check if closing up by joining $t_{2i}$ to $t_1$ will give a gain value better than the best seen previously, let $y_i^* = (t_{2i}, t_1)$. If G_{i - 1} + g_i^* > G^*$, set $G^* = G_{i - 1} + g_i^*$
              if $G_i \ge G^*$, repeat from 1.a
          2. Set $T = T \\ X \cup Y$ and repeat.

When $i >= 2$ the choice if $x_i$, $y_i$ is subject to more constraints than when $i = 1$, such constraints are

 (a) $x_i$ and $y_i$ cannot be part of the alternating trail
 (b) the overall gain, $G_i = \sum_1^i  \SetAlgoNoLine g_j$ is strictly bigger than 0
 (c) the new $y_i$ must allow the breaking of an $x_{i + 1}$.

This algorithm can be reinterpreted as a recursive algorithm, where it is built and explored a tree with alternating trails of increasing size, when necessary the algorithm backtracks, meaning it explores other branches of the recursion tree.

### Recursive algorithm

This LK pseudo-code takes inspiration from the following Wikipedia article: [@noauthor_linkernighan_2023].
In the following I explain the recursive version of LK, which is already rewritten in its iterative version, by means of a stack. The idea is the following:

 - start from a tour $T$
 - for all possible starting nodes $u$, save its position in the alternating trail, $i$, and the gain so far, $G_i = \sum_1^i g_j$, call it $g$
 - explore recursively all possible ways to build an alternating trail, the algorithms prunes, or explores further the tree of alternating trails, and chooses the best improvement over the initial starting tour $T$
 - if the algorithm did not find any improvement, retry with a different initial tour.

In this algorithm I use sets of edges: in the following pseudo-code the ordering of the elements of sets is not important, because they are named properly, the implementation uses vectors, but the operations between sets are the same.

<!-- TODO: specify what F is, and how to update it -->
\begin{algorithm}
  \SetKwFunction{Peek}{peek}
  \SetKwFunction{Pop}{pop}
  \SetKwFunction{Push}{push}
  \SetKwFunction{Clear}{clear}
  \KwData{A graph $G$, a tour $T$.\\The graph $G$ supports a function $V(G)$ which returns the list of nodes of $G$, moreover $c: E\to \mathbb{N}$ is a function that returns the cost of an edge.\\A stack data structure is available, it supports the usual operations such as push, pop, peek, and clear.\\
  Parameters $p_1$, $p_2$ could be retrieved from the global context.}
  \KwResult{An improved $T$}
  \BlankLine
  $s \gets$ initialize empty stack;

  $G^* \gets 0$; $F^* \gets \emptyset$\tcc*[r]{best improvement and best alternating trail so far}
  $F\gets\emptyset$\tcc*[r]{the alternating trail the algorithm is currently visiting}
  
  \Repeat{$G^* = 0$}{
    \ForEach{$u\in V(G)$ s.t. $v_i u\in T$}{
      \Push{$s$, $(u, 0, 0)$}\;
    }
    \While{$s$ is not empty}{
      $(v_i, i, g) \gets$ \Pop{$s$}\;
      $F \gets F \cup v_i$\tcc*[r]{overwrites a previous $v_i$ that could be in $F$}
      \uIf{$i$ is even}{
        \ForEach{$u\in V(G)$ s.t. $(v_i, u) \in T\setminus F$}{
          \If{$i\leq p_2 \vee ((u, v_0)\notin T \cup F \cup \{(v_i, u)\}) \wedge T\Delta (F \cup \{(v_i, u),  (u, v_0)\})$ is a tour}{
            \Push{$s$, $(u, i + 1, g + c(v_i u))$}\;
          }
        }
      }
      \Else{
        \If{$g - c(v_i,v_0)\ge G^*\wedge T\Delta (F\cup \{(v_i, v_0)\})$ is a tour}{
          $F^* \gets F \cup \{(v_i, v_0)\}$; $G^* \gets g - c(v_i, v_0)$\;
          \ForEach{$u\in V(G)$ such that $g\ge c(v_i, u)\wedge (v_i, u)\notin T\cup F$}{
            \Push{$s$, $(u, i + 1, g - c(v_i, u))$}\;
          }
        }
      }
      
      $(u, j, g) \gets$ \Peek{$s$}\;
      \If{$i \leq j$}{
        \uIf{$G^*\ge 0$} {
          $T \gets T \Delta F$; \Clear{$s$}\;
        }
        \ElseIf{$i\ge p_1$}{
          \lForEach{$(u, j, g) \gets$\Peek{$s$} s.t. $j\ge p_1$}{\Pop{$s$}}
        }
      }

    }
  }
  \Return{T}
  \caption{Lin-Kernighan\label{LK}}
\end{algorithm}

### C++ implementation

The C++ implementation is a straight forward implementation of the pseudo-code from \ref{LK}.
The only things left to be specified are the data structures used, and the implementation of the function which returns whether if a given set of edges $T$ is a tour.

The data structures used are the standard implementations of vector, set, and deque.

## Issues of the implementation

1. The set data structure has a high computational complexity in lookup, insertion, deletion of entries, but it greatly simplifies the development process: it allows to easily translate the pseudo-code into the actual C++ implementation. Real world implementations use data structures such as linked lists or arrays of nodes.

2. Optimizations from the original LK paper are missing.

3. I did not take into account space complexity: when LK was published it was a problem, and instances with sizes above 100 were a problem. As of now, storage utilization is certainly not the first concern. A possible solution, that would reduce storage utilization, while not increasing the complexity of the implementation, is to use persistent data structures [@driscoll-1986]. Persistent data structures would allow having multiple versions of the same data structure, without having copies of such data structures. C++ offers persistent data structures only through third party libraries.

# MILP TSP

# Tests

## Test data

## Test results

| instance | nodes | minCost | maxCost | Exact cost | Relative error (percentage) | Relative std | meanCpuTime |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 010_s_0 | 10 | 25.167 | 25.167 | 25.167 | 0,00000% | 0,00000% | 0,00135 |
| 010_s_1 | 10 | 22.416 | 22.416 | 22.416 | 0,00000% | 0,00000% | 0,00075 |
| 010_s_2 | 10 | 29.489 | 29.489 | 29.489 | 0,00000% | 0,00000% | 0,00051 |
| 010_s_3 | 10 | 22.950 | 22.950 | 22.950 | 0,00000% | 0,00000% | 0,00063 |
| 010_s_4 | 10 | 25.339 | 25.339 | 25.339 | 0,00000% | 0,00000% | 0,00038 |
| 010_s_5 | 10 | 27.524 | 27.524 | 27.524 | 0,00000% | 0,00000% | 0,00091 |
| 010_s_6 | 10 | 29.547 | 29.547 | 29.547 | 0,00000% | 0,00000% | 0,00157 |
| 010_s_7 | 10 | 26.314 | 26.314 | 26.314 | 0,00000% | 0,00000% | 0,00046 |
| 010_s_8 | 10 | 23.121 | 23.121 | 23.121 | 0,00000% | 0,00000% | 0,00076 |
| 010_s_9 | 10 | 29.610 | 30.525 | 29.610 | 0,00000% | 1,19902% | 0,00121 |
| 020_sm_0 | 20 | 48.693 | 48.693 | 48.693 | 0,00000% | 0,00000% | 0,00256 |
| 020_sm_1 | 20 | 53.658 | 53.925 | 53.658 | 0,00000% | 0,20548% | 0,01005 |
| 020_sm_2 | 20 | 45.384 | 45.384 | 45.384 | 0,00000% | 0,00000% | 0,00436 |
| 020_sm_3 | 20 | 49.323 | 50.240 | 49.323 | 0,00000% | 0,64334% | 0,00550 |
| 020_sm_4 | 20 | 44.677 | 44.677 | 44.677 | 0,00000% | 0,00000% | 0,00369 |
| 020_sm_5 | 20 | 42.289 | 42.289 | 42.289 | 0,00000% | 0,00000% | 0,00271 |
| 020_sm_6 | 20 | 51.353 | 52.232 | 51.353 | 0,00000% | 0,48748% | 0,00610 |
| 020_sm_7 | 20 | 45.288 | 45.818 | 45.288 | 0,00000% | 0,57838% | 0,00739 |
| 020_sm_8 | 20 | 45.257 | 45.620 | 45.257 | 0,00000% | 0,38981% | 0,01005 |
| 020_sm_9 | 20 | 48.117 | 48.117 | 48.117 | 0,00000% | 0,00000% | 0,00319 |
| 040_m_0 | 40 | 92.802 | 97.977 | 92.802 | 0,00000% | 1,79416% | 0,04897 |
| 040_m_1 | 40 | 91.263 | 93.463 | 91.263 | 0,00000% | 0,74251% | 0,05195 |
| 040_m_2 | 40 | 90.792 | 92.719 | 90.792 | 0,00000% | 0,83133% | 0,08687 |
| 040_m_3 | 40 | 91.971 | 93.714 | 91.971 | 0,00000% | 0,61888% | 0,02145 |
| 040_m_4 | 40 | 94.764 | 96.966 | 94.764 | 0,00000% | 0,75854% | 0,07614 |
| 040_m_5 | 40 | 95.021 | 97.579 | 94.143 | 0,93262% | 0,74496% | 0,04539 |
| 040_m_6 | 40 | 92.821 | 95.587 | 91.063 | 1,93053% | 0,84861% | 0,04020 |
| 040_m_7 | 40 | 101.209 | 103.441 | 101.150 | 0,05833% | 0,62340% | 0,03608 |
| 040_m_8 | 40 | 81.903 | 86.586 | 81.903 | 0,00000% | 1,59728% | 0,02212 |
| 040_m_9 | 40 | 92.821 | 94.845 | 92.821 | 0,00000% | 0,64909% | 0,04667 |
| 080_ml_0 | 80 | 195.848 | 203.264 | 192.792 | 1,58513% | 1,20772% | 0,30637 |
| 080_ml_1 | 80 | 193.021 | 198.346 | 192.077 | 0,49147% | 0,88300% | 0,31063 |
| 080_ml_2 | 80 | 187.872 | 200.665 | 187.872 | 0,00000% | 2,07301% | 0,20446 |
| 080_ml_3 | 80 | 188.052 | 197.480 | 187.696 | 0,18967% | 1,33475% | 0,20163 |
| 080_ml_4 | 80 | 184.996 | 193.609 | 184.996 | 0,00000% | 1,08826% | 0,20483 |
| 080_ml_5 | 80 | 191.032 | 197.632 | 190.548 | 0,25400% | 1,10485% | 0,20869 |
| 080_ml_6 | 80 | 189.020 | 195.492 | 188.204 | 0,43357% | 1,15415% | 0,18789 |
| 080_ml_7 | 80 | 194.320 | 202.068 | 193.148 | 0,60679% | 1,15512% | 0,21281 |
| 080_ml_8 | 80 | 192.792 | 196.436 | 189.504 | 1,73506% | 0,55304% | 0,28244 |
| 080_ml_9 | 80 | 191.008 | 197.253 | 190.652 | 0,18673% | 0,84817% | 0,41513 |
| 100_l_0 | 100 | 243.416 | 250.604 | 240.460 | 1,22931% | 0,68690% | 0,57485 |
| 100_l_1 | 100 | 237.989 | 245.761 | 236.104 | 0,79838% | 0,92212% | 0,50675 |
| 100_l_2 | 100 | 233.252 | 238.908 | 231.468 | 0,77073% | 0,70311% | 0,56689 |
| 100_l_3 | 100 | 235.492 | 241.148 | 234.192 | 0,55510% | 0,74767% | 0,42911 |
| 100_l_4 | 100 | 233.608 | 240.749 | 232.436 | 0,50422% | 1,06689% | 0,46774 |
| 100_l_5 | 100 | 231.976 | 240.436 | 229.836 | 0,93110% | 1,05791% | 0,58532 |
| 100_l_6 | 100 | 232.460 | 238.600 | 230.344 | 0,91863% | 0,76976% | 0,38482 |
| 100_l_7 | 100 | 235.364 | 243.473 | 232.536 | 1,21616% | 0,99952% | 0,55136 |
| 100_l_8 | 100 | 236.460 | 244.408 | 233.044 | 1,46582% | 0,95200% | 0,38117 |
| 100_l_9 | 100 | 236.204 | 242.576 | 234.676 | 0,65111% | 0,81327% | 0,41010 |

: The table displays the min, max, exact cost obtained by running the algorithm Lin-Kernighan 10 times for each instance. It shows relative standard deviation, and relative error, with respect to the cost, and the exact cost, respectively. The table contains data about running times as well, in seconds.\label{tab:results}

| Nodes | Mean cpu time | Mean relative error | Max relative error | Relative std dev cpu time |
| --- | --- | --- | --- | --- |
| 10 | 0,00085 | 0,00000% | 0,00000% | 0,47178 |
| 20 | 0,00556 | 0,00000% | 0,00000% | 0,50771 |
| 40 | 0,04758 | 0,29215% | 1,93053% | 0,43771 |
| 80 | 0,25349 | 0,54824% | 1,73506% | 0,28946 |
| 100 | 0,48581 | 0,90406% | 1,46582% | 0,16745 |

: Running times  and results of the instances tested, grouped by size of instance, using the Lin-Kernighan heuristic implementation. The table displays the mean and max relative error, with respect to the exact solutions obtained via MILP, and the mean CPU time.
and the CPU time, in seconds. \label{tab:results-summary}

# Conclusion

# References
