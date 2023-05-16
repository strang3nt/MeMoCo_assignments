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

  \usepackage{algorithm2e}
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

1. Let $T$ be a random starting tour.
2. For all $t \in T$, let $t$ be $t_1$ do:
  1. Set $G^* = 0$. For all edges adjacent to $t_1$ such that they are in the tour, let one such edge be $x_1$ and let $t_2$ be the other node. Set $i = 1$. For all $x_1$, until $G^* = 0$, do:
    1. Compute all possible $y_1$, such that $y_1 = (t_2, t_3)$, and $y_1\notin T$, ordered by improvement $g_1 > 0$, descending. For all $y_1$ do (6.c):
      1. Let $i = i + 1$, and compute all possible $x_2$ such that if it is joined to the first node of the tour, the resulting configuration is a tour, until $G^* = 0$ do:
        1. Compute all $y_2$, order of increasing cost $c(y_2)$, following conditions 4.c, 4.d, 4.e, which are described below:
          1.a Repeat until it is not possible to build $x_i$, $y_i$
            1. Let $i = i + 1$, and choose $x_i$, which currently joins $t_{2i - 1}$ to $t_{2i}$, and $y_i$ as follows:
              1. let $x_i$ be such that if $t_{2i}$ is joined to $t_1$, the resulting configuration is a tour
              2. let $y_i$ be some available link available at the node $t_{2i}$ shared with $x_i$, subject to 4.c, 4.d, 4.e
              3. before $y_i$ is constructed, check if closing up by joining $t_{2i}$ to $t_1$ will give a gain value better than the best seen previously, let $y_i^* = (t_{2i}, t_1)$. If G_{i - 1} + g_i^* > G^*$, set $G^* = G_{i - 1} + g_i^*$
              if $G_i \ge G^*$, repeat from 1.a
          1. Set $T = T \\ X \cup Y$ and repeat.

When $i >= 2$ the choice if $x_i$, $y_i$ is subject to more constraints than when $i = 1$, such constraints are

 (a) $x_i$ and $y_i$ cannot be part of the alternating trail
 (b) the overall gain, $G_i = \sum_1^i g_j$ is strictly bigger than 0
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
lin_kernighan(G)
\begin{algorithm}

  \SetKwFunction{Peek}{peek}\SetKwFunction{Pop}{pop}\SetKwFunction{Push}{push}
  \SetKwInOut{Input}{Input}\SetKwInOut{Output}{Output}
  \Input{A graph $G$, a tour $T$}
  \Output{An improved $T$}

  $stack \gets \{\,\}$\;

  $g^* \gets 0$; $F^* \gets \emptyset$; $F\gets\emptyset$\;
  
  \Repeat{$g^* = 0$}{
    \ForEach{$u\in V(G)$ s.t. $v_i u\in T$}{
      \Push{$stack$, $(u, 0, 0)$}\;
    }
    \While{$stack$ is not empty}{
      $(v_i, i, g) \gets$ \Pop{$stack$}\;
      $F \gets F \cup v_i$\tcc*[r]{overwrites a previous $v_i$ that could be in $F$}
      \If{$i$ is even}{
        \ForEach{$u\in V(G)$ s.t. $v_i u \in T\setminus F$}{
          \If{$i\leq p_2 \vee (u v_0\notin T \cup F \cup \{v_i u\}) \wedge T\Delta (F \cup \{v_i u,  u v_0\})$ is a tour}{
            \Push{$stack$, $(u, i + 1, g + c(v_i u))$}
          }
        }
      }
      \Else{
        \If{$g - c(v_i v_0)\ge g^*\wedge T\Delta (F\cup \{v_i v_0)\}$ is a tour}{
          $F^* \gets F \cup \{v_i v_0\}$; $g^* \gets g - c(v_i v_0)$\;
          \ForEach{$u\in V(G)$ such that $g\ge c(v_i u)\wedge v_i u\notin T\cup F$}{
            \Push{$stack$, $(u, i + 1, g - c(v_i u))$}
          }
        }
      }

      
      
      $(u, j, g) \gets$ \Peek{$stack$}\;
      \If{$i \leq j$}{
        \If{$g^*\ge 0$} {
          $T \gets T \Delta F$; $stack \gets \{ \}$\;
        }
        \ElseIf{$i\ge p_1$}{
          \lForEach{$(u, j, g) \gets$\Peek{$stack$} s.t. $j\ge p_1$}{\Pop{$stack$}}
        }
      }

    }
  }
  \Return{T}

\end{algorithm}

## Issues of the implementation

1. The set data structure has a high computational complexity in lookup, insertion, deletion of entries, but it greatly simplifies the development process: it allows to easily translate the pseudo-code into the actual C++ implementation. Real world implementations use data structures such as linked lists or arrays of nodes.

2. Optimizations from the original LK paper are missing.

3. I did not take into account space complexity: when LK was published it was a problem, and instances with sizes above 100 were a problem. As of now, storage utilization is certainly not the first concern. A possible solution, that would reduce storage utilization, while not increasing the complexity of the implementation, is the use of persistent data structures [@driscoll-1986]. Persistent data structures would allow having multiple versions of the same data structure, without having copies of such data structures. C++ offers persistent data structures only through third party libraries.

# Tests

## Test data

## Test results


  Instance     Nodes   Edges    Result  User time(seconds)  CPU time(seconds)
  ---------- ------- ------- --------- ------------------- ------------------
  pka010       10     100     30        0.00244498          0.002447
  pka020       20     400     60.0061   0.0122151           0.012175
  pka040       40     1600    152.087   1.32314             1.32123
  pka060       60     3600    225       3.53624             3.53195
  pka120       120    14400   472.284   154.186             153.981
  pma010       10     100     66        0.00215387          0.002148
  pma020       20     400     72.4775   0.0483072           0.048194
  pma040       40     1600    155.649   0.466176            0.465509
  pma060       60     3600    237.082   4.32367             4.31767
  pma120       120    14400   488.222   108.883             108.722
  xqf010       10     100     81.8798   0.00622201          0.00619
  xqf020       20     400     115.498   0.057502            0.057401
  xqf040       40     1600    157.512   0.11927             0.119111
  xqf060       60     3600    225.09    2.80433             2.80067
  xqf120       120    14400   486.651   100.48              100.335
  xqg010       10     100     98.6212   0.0048511           0.00484
  xqg020       20     400     133.606   0.027863            0.027813
  xqg040       40     1600    194.575   0.550078            0.549321
  xqg060       60     3600    257.833   1.79939             1.79699
  xqg120       120    14400   586.468   103.908             103.761

: Run-time  and results of the instances tested, using the Lin-Kernighan heuristic implementation. The table displays the weight of the TSP tour, the user time
and the CPU time, in seconds. \label{tab:results}


# Conclusion

# References
