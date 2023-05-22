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

  \usepackage[ruled]{algorithm2e}
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
LK is a very successful attempt at solving TSP: it is able to solve instances with unknown optimal solutions (as of 1971, when Lin and Kernighan wrote the paper), with near optimal results. And it was able to do so within a $O(n^{2.2})$ time complexity.
The Lin-Kernighan heuristic is the starting point of a more advanced heuristic called Lin-Kernighan-Helsgaun. The latter is to this day one of the best performing algorithms for TSP [@HELSGAUN2000106].

In the first sections of the paper I show the LK implementation. Then I compare LK with a MILP model for TSP. The comparison shows the difference in computation time, optimality of the solutions. 

# Lin-Kernighan heuristic

## Background

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

Many implementation details are missing, still this should be a good starting point, and a good outline of LK.

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

This LK pseudo-code takes inspiration from the following Wikipedia page: <https://en.wikipedia.org/wiki/Lin-Kernighan_heuristic>
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

3. I did not take into account space complexity: when LK was published it was a problem, and instances with sizes above 100 were a problem. As of now, storage utilization is certainly not the first concern. A possible solution, that would reduce storage utilization, while not increasing the complexity of the implementation, is the use of persistent data structures [@driscoll-1986]. Persistent data structures would allow having multiple versions of the same data structure, without having copies of such data structures. C++ offers persistent data structures only through third party libraries.

# Tests

## Test data

## Test results

| Instance | Min cost | Max cost | Exact cost | Rel std | Rel error | Min CPU time | Max CPU time |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 010_s_0 | 3.080.494 | 3.080.494 | 3.080.494 | 0,00% | 0,00% | 0,00061 | 0,00135 |
| 010_s_1 | 3.097.542 | 3.097.542 | 3.097.542 | 0,00% | 0,00% | 0,00037 | 0,00066 |
| 010_s_2 | 2.473.785 | 2.473.785 | 2.473.785 | 0,00% | 0,00% | 0,00064 | 0,00124 |
| 010_s_3 | 2.109.674 | 2.109.674 | 2.109.674 | 0,00% | 0,00% | 0,00031 | 0,00063 |
| 010_s_4 | 2.671.884 | 2.671.884 | 2.671.884 | 0,00% | 0,00% | 0,00078 | 0,00170 |
| 010_s_5 | 2.604.758 | 2.604.758 | 2.604.758 | 0,00% | 0,00% | 0,00048 | 0,00073 |
| 010_s_6 | 2.382.952 | 2.382.952 | 2.382.952 | 0,00% | 0,00% | 0,00031 | 0,00097 |
| 010_s_7 | 2.541.640 | 2.541.640 | 2.541.640 | 0,00% | 0,00% | 0,00021 | 0,00031 |
| 010_s_8 | 2.648.528 | 2.648.528 | 2.648.528 | 0,00% | 0,00% | 0,00020 | 0,00038 |
| 010_s_9 | 2.648.127 | 2.648.127 | 2.648.127 | 0,00% | 0,00% | 0,00061 | 0,00101 |
| 020_sm_0 | 4.378.036 | 4.378.036 | 4.378.036 | 0,00% | 0,00% | 0,00223 | 0,00711 |
| 020_sm_1 | 4.724.210 | 4.819.348 | 4.724.210 | 0,80% | 0,00% | 0,00379 | 0,01234 |
| 020_sm_2 | 4.480.164 | 4.480.164 | 4.480.164 | 0,00% | 0,00% | 0,00313 | 0,00942 |
| 020_sm_3 | 4.731.912 | 4.758.530 | 4.731.912 | 0,20% | 0,00% | 0,00683 | 0,01517 |
| 020_sm_4 | 4.414.432 | 4.502.300 | 4.414.432 | 0,60% | 0,00% | 0,00397 | 0,01285 |
| 020_sm_5 | 4.537.967 | 4.537.967 | 4.537.967 | 0,00% | 0,00% | 0,00496 | 0,01070 |
| 020_sm_6 | 5.005.086 | 5.022.134 | 5.005.086 | 0,10% | 0,00% | 0,00282 | 0,00442 |
| 020_sm_7 | 4.403.364 | 4.491.232 | 4.403.364 | 0,77% | 0,00% | 0,00375 | 0,00910 |
| 020_sm_8 | 5.129.022 | 5.129.022 | 5.129.022 | 0,00% | 0,00% | 0,00278 | 0,00812 |
| 020_sm_9 | 4.579.100 | 4.579.100 | 4.579.100 | 0,00% | 0,00% | 0,00191 | 0,00373 |
| 040_m_0 | 9.448.877 | 9.678.489 | 9.448.877 | 0,75% | 0,00% | 0,02818 | 0,06232 |
| 040_m_1 | 9.400.019 | 9.850.444 | 9.400.019 | 1,31% | 0,00% | 0,08694 | 0,16635 |
| 040_m_2 | 9.427.142 | 9.568.886 | 9.427.142 | 0,63% | 0,00% | 0,03736 | 0,09171 |
| 040_m_3 | 10.055.586 | 10.623.375 | 10.055.586 | 1,76% | 0,00% | 0,03044 | 0,06612 |
| 040_m_4 | 9.364.024 | 9.723.776 | 9.364.024 | 1,47% | 0,00% | 0,03270 | 0,06699 |
| 040_m_5 | 9.559.108 | 9.888.795 | 9.559.108 | 1,16% | 0,00% | 0,03747 | 0,12334 |
| 040_m_6 | 8.899.356 | 9.278.560 | 8.899.356 | 1,20% | 0,00% | 0,02276 | 0,07459 |
| 040_m_7 | 9.003.871 | 9.331.987 | 9.003.871 | 1,22% | 0,00% | 0,03631 | 0,06333 |
| 040_m_8 | 9.482.308 | 9.860.502 | 9.482.308 | 1,41% | 0,00% | 0,02700 | 0,06318 |
| 040_m_9 | 9.665.017 | 9.948.073 | 9.586.823 | 0,85% | 0,82% | 0,03423 | 0,12785 |
| 080_ml_0 | 19.023.869 | 19.876.857 | 18.906.712 | 1,29% | 0,62% | 0,10885 | 0,30774 |
| 080_ml_1 | 18.752.603 | 19.219.373 | 18.716.974 | 0,81% | 0,19% | 0,13239 | 0,26792 |
| 080_ml_2 | 19.149.973 | 19.901.444 | 19.019.916 | 1,07% | 0,68% | 0,13709 | 0,37714 |
| 080_ml_3 | 19.430.186 | 20.148.658 | 19.361.558 | 1,04% | 0,35% | 0,14203 | 0,35215 |
| 080_ml_4 | 18.522.858 | 19.501.443 | 18.500.129 | 1,49% | 0,12% | 0,19014 | 0,48499 |
| 080_ml_5 | 19.527.244 | 20.102.759 | 19.397.187 | 1,08% | 0,67% | 0,12423 | 0,23871 |
| 080_ml_6 | 19.621.230 | 20.281.344 | 19.504.073 | 1,12% | 0,60% | 0,15784 | 0,51541 |
| 080_ml_7 | 18.854.230 | 20.031.501 | 18.854.230 | 1,88% | 0,00% | 0,10048 | 0,32143 |
| 080_ml_8 | 19.244.401 | 20.158.928 | 19.195.872 | 1,30% | 0,25% | 0,14748 | 0,33659 |
| 080_ml_9 | 18.467.129 | 19.501.444 | 18.385.601 | 1,76% | 0,44% | 0,16469 | 0,37468 |
| 100_l_0 | 23.695.560 | 24.447.031 | 23.377.088 | 0,92% | 1,36% | 0,28460 | 0,62008 |
| 100_l_1 | 23.978.403 | 24.595.329 | 23.565.503 | 0,87% | 1,75% | 0,32896 | 0,66648 |
| 100_l_2 | 23.598.501 | 24.424.302 | 23.399.817 | 1,00% | 0,85% | 0,27532 | 0,64897 |
| 100_l_3 | 23.516.974 | 24.078.172 | 23.333.900 | 0,74% | 0,78% | 0,27729 | 0,58589 |
| 100_l_4 | 23.032.816 | 23.692.930 | 22.902.759 | 0,89% | 0,57% | 0,25870 | 0,61390 |
| 100_l_5 | 23.496.875 | 24.447.031 | 23.496.875 | 1,27% | 0,00% | 0,28299 | 0,71253 |
| 100_l_6 | 23.504.074 | 24.177.088 | 23.351.288 | 0,85% | 0,65% | 0,37720 | 0,72443 |
| 100_l_7 | 23.331.189 | 24.212.717 | 23.318.289 | 1,04% | 0,06% | 0,21816 | 0,67756 |
| 100_l_8 | 23.991.303 | 24.638.517 | 23.812.717 | 0,87% | 0,75% | 0,36708 | 0,60183 |
| 100_l_9 | 23.845.716 | 24.775.772 | 23.598.502 | 1,28% | 1,05% | 0,37966 | 0,66030 |

: The table displays the min, max, exact cost obtained by running the algorithm Lin-Kernighan 10 times for each instance. It shows relative standard deviation, and relative error, with respect to the cost, and the exact cost, respectively. The table contains data about running times as well, in seconds.\label{tab:results}


| Nodes | Mean cpu time | Mean relative error | Max relative error |
| --- | --- | --- | --- |
| 10 | 0,00063 | 0,00% | 0,00% |
| 20 | 0,00584 | 0,00% | 0,00% |
| 40 | 0,05844 | 0,08% | 0,82% |
| 80 | 0,23702 | 0,39% | 0,68% |
| 100 | 0,44255 | 0,78% | 1,75% |

: Running times  and results of the instances tested, grouped by size of instance, using the Lin-Kernighan heuristic implementation. The table displays the mean and max relative error, with respect to the exact solutions obtained via MILP, and the mean CPU time.
and the CPU time, in seconds. \label{tab:results-summary}

# Conclusion

# References
