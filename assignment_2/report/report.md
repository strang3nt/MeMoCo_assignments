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
  \makeatletter
  \def\fps@figure{H}
  \makeatother
codeBlockCaptions: True
...

# Introduction

A company produces boards with holes used to build electric panels. 
The company wants to minimize the total drilling time for each board, taking into account that the time needed for making an hole is the same and constant for
all the holes. This is an instance of the Traveling Salesperson Problem (TSP).

In this report I show the implementation of a heuristic for the Traveling Salesperson Problem (TSP).
The heuristic is the Lin-Kernighan (LK) heuristic for TSP [@lin_effective_1973].
LK is a very successful attempt at solving TSP: it was able to solve instances with, at the time (1971), unknown optimal solutions, with near optimal results. And it was able to do so within a $O(n^{2.2})$ time complexity.
The Lin-Kernighan heuristic is the starting point for a more advanced heuristic called Lin-Kernighan-Helsgaun. The latter is, to this day, one of the best performing algorithms for TSP [@HELSGAUN2000106].

In the first section of the paper I show the LK implementation. The following section describes an algorithm that computes the exact solution, such algorithm is based on Mixed Integer Linear Programming (MILP), and it uses the CPLEX API by IBM. Then I compare both of the aforementioned implementation. The comparison shows the difference in computation time, and optimality of the solutions. 

# TSP instances for the drilling problem

## Generation of instances

Proper instances for the drilling problem are created via a script.
The instance generator produces files according to the following input:

 1. size of board, height and length
 2. distance between holes coordinates
 3. size of instance and number of instances.

The script fills a two-dimensional plane with as many coordinates as possible, distributed in a grid, where holes are separated by 
the distance input in (2). Then it uses these coordinates to select randomly a number of points, which are then saved in a file, and
this process is repeated with respect to the input provided in step (3).
The files come in a `.tsp` extension, they are named with the following pattern: `userprovidedname_numberofinstance.tsp`.
The following is an example of a file, the first line is the number of nodes, the next lines contain the number of the node,
and its x, y coordinates in the plane:

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

I decided to use a precision in the order of $10^{-5}$, every input is integer, as well as the distance between points, which is calculated using
the euclidean distance, is rounded to the nearest integer.

A `.tsp` file is then parsed to a graph object, which is then used by the CPLEX API and the MILP implementation.

> Note that a graph is supposed to be complete, each node is connected to each end every other node in the graph.

## A solution for the TSP drilling problem

The results are euclidean distances, which is not a correct answer for the question at hand, 
which is how much time it takes for a mechanical arm to drill
a board. I decided to use distances nonetheless, since they can be easily converted to
time measures, once the drilling speed and the speed of movement of the arm is known.

The unit of measure is $10^{-5}$ meters, which is probably more precise than a real drilling machine would support.
Values are integers.
For example if a solution is $25.167$, it means that the length of the best possible hamiltonian path is $25,167$ centimeters.

# Lin-Kernighan heuristic

## Background

Let $G = (V, E)$ be an undirected, complete graph, where $V$ is the set of nodes, and $E$ is the set of edges where $(u, v) \in E$ is an edge.
Let $c: E \to\mathbb{Z}$ be a total function that given an edge $(u, v)$ returns its cost.
A tour is an hamiltonian cycle, a path in the graph which visits all nodes exactly once, except the starting node, which is visited twice.
Given a tour $T$ an alternating trail is a list of edges such that they are alternatingly in $T$ and not in $T$. An edge $(u, v)$ is said to be adjacent to a particular node if that node is either $u$ or $v$, any number of edges is said to be adjacent if they share one common node.

I will use set operations such as union, $\cup$, set difference $\setminus$ and set symmetric difference, $\Delta$.

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

\begin{algorithm}
  \KwData{A graph $G$, a tour $T$.\\$V$ and $E$ are the set of nodes and the set of edges of $G$, respectively, moreover $c: E\to \mathbb{N}$ is a function that returns the cost of an edge.\\}
  \KwResult{An improved $T$.}
  \BlankLine

  \ForEach{$t\in V$}{
    $t_1\gets t$\;
    $G^*\gets 0$\;
    \ForEach{$e\in E$ adjacent to $t_1$, $e\in T$}{
      $x_1\gets e$\;
      $t_2\gets$ the edge in $x_1$ which is not $t_1$\;
      $i \gets 1$\;
      \ForEach{$y_1$ such that $y_1\notin T$, ordered by improvement $g_1 > 0$, descending}{
        $i\gets 2$\;
        \ForEach{$x_2$ s.t. $x_2\in E$, $x_2\in T$, adjacent to $y_1$, if it is joined to the first node of the tour, the resulting configuration is a tour, and $G^* = 0$}{
          \ForEach{$y_2$, computed in order of increasing cost $c(y_2)$, such that $y_2$ satisfies conditions (a), (b), (c)}{   
            \Repeat{it is not possible to build $x_i$, $y_i$, or $G_i \leq G^*$}{
              $i\gets i + 1$\;
              $x_i\gets (t_{2i - 1}, t_{2i})$, choose $t_{2i}$ such that if it is joined to $t_1$, the resulting configuration is a tour\;
              $y_i^* \gets (t_{2i}, t_1)$\tcc*[r]{check if closing up by joining $t_{2i}$ to $t_1$ gives a gain value better than the best seen previously}
              \If{$G_{i - 1} + g_i^* > G^*$}{
                $G^* \gets G_{i - 1} + g_i^*$\;
              }
            }
          }
        }
      }
    }
    $T\gets T \setminus X \cup Y$\tcc*[r]{where $X=\{x_0,...,x_n\}$, $Y=\{y_0,...,y_n\}$}
  }
  \caption{Lin-Kernighan iterative version\label{alg:lkIterative}}
\end{algorithm}

When $i \geq 2$ the choice if $x_i$, $y_i$ is subject to the
following constraints:

 (a) $x_i$ and $y_i$ cannot already be part of the alternating trail
 (b) the overall gain, $G_i = \sum_1^i  \SetAlgoNoLine g_j$ is strictly bigger than 0
 (c) the new $y_i$ must allow the breaking of an $x_{i + 1}$.

This algorithm can be reinterpreted as a recursive algorithm, where it is built and explored a tree with alternating trails of increasing size, and when necessary the algorithm backtracks, meaning it explores other branches of the recursion tree.

### Recursive algorithm

This LK pseudo-code takes inspiration from the following Wikipedia article: [@noauthor_linkernighan_2023].
In \ref{LK} I show the recursive version of LK, which is already rewritten in its iterative version, by means of a stack. The idea is the following:

 - start from a tour $T$
 - for all possible starting nodes $u$, save their position in the alternating trail, $i$, and the gain so far, $G_i = \sum_1^i g_j$, call it $g$
 - explore recursively all possible ways to build an alternating trail, the algorithms prunes, or explores further the tree of alternating trails, and chooses the best improvement over the initial starting tour $T$
 - if the algorithm did not find any improvement, retry with a different initial tour.

In this algorithm I use sets of edges: in the following pseudo-code the ordering of the elements of sets is not important, because they are named properly, the implementation uses vectors, but the operations between sets are the same.

\begin{algorithm}
  \SetKwFunction{Peek}{peek}
  \SetKwFunction{Pop}{pop}
  \SetKwFunction{Push}{push}
  \SetKwFunction{Clear}{clear}
  \KwData{A graph $G$, a tour $T$.\\The graph $G$ supports a function $V(G)$ which returns the list of nodes of $G$, moreover $c: E\to \mathbb{N}$ is a function that returns the cost of an edge.\\A stack data structure is available, it supports the usual operations such as push, pop, peek, and clear.\\
  Parameters $p_1$, $p_2$ could be retrieved from the global context.}
  \KwResult{An improved $T$.}
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

The C++ implementation is a straight forward implementation of \ref{LK}.
The only things left to be specified are the data structures used, and the implementation of the function which returns whether if a given set of edges $T$ is a tour.

The data structures used are the standard implementations of vector, set, and deque.
The alternating trail, and more in general any trail (remember that a tour is a trail), is represented by a class `Tour`, whose interface
is \ref{lst:trailClass}.

Listing: Interface of `Trail` class.

```{#lst:trailClass .cpp}
class Trail {

  private:
    std::set<std::tuple<int, int>> edges;

  public:
    /* constructors */
    Trail(
      const std::set<std::tuple<int, 
      int>>& _alternatingTrail): edges(_alternatingTrail) {}
    Trail(const std::vector<int>& edges, int i);
    Trail(): edges(std::set<std::tuple<int,int>>()) {}

    static const std::unique_ptr<Trail> getRandomTour(
      const std::vector<std::vector<int>>& w);
    static const std::tuple<int, int> makeTuple(int u, int v);

    bool isTour(int n) const;

    /* utility functions */
    int cost(const Graph& g) const;
    int size();
    bool contains(const std::tuple<int, int>& i) const;
    void insert(const std::tuple<int, int>& i);
    void erase(const std::tuple<int, int>& i);

    /* set operations */
    std::unique_ptr<Trail> trailUnion(const Trail& otherTrail) const;
    std::unique_ptr<Trail> trailDifference(const Trail& otherTrail) const;
    std::unique_ptr<Trail> trailSymmetricDifference(const Trail& otherTrail) const;
};
```

LK needs to check frequently whether if a given trail is a tour, `Trail::isTour`, in \ref{lst::isTour} does exactly that.
`isTour` takes as input the size of the tour, then walks along the adjacent edges, if during the walk the function did
not return to the initial node, or it couldn't walk along all edges of the trail (because they weren't all adjacent) then
the trail is not a tour.

Listing: Function `isTour` from Trail class.

```{#lst:isTour .cpp}
bool isTour() const {

  std::vector<bool> visited(edges.size(), false);
  int j = 0;
  for(int s = 0; s < edges.size(); ++s) {

    const auto& it = std::find_if(
      this->edges.begin(),   
      this->edges.end(),
      [s, j, &visited, n](const auto& edge){ 
        const auto [x, y] = edge;
        int i = j == x ? y : x;
        return (x == j || y == j) && (!visited.at(i) || (s == n - 1 && i == 0)); });
    if(it != this->edges.end()) {
      visited.at(j) = true;
      const auto [x, y] = *it;
      j = x == j ? y : x;
    }
  }
  
  return std::find(visited.begin(), visited.end(), false) == visited.end();
}
```

## Issues of the implementation

1. The set data structure has a high computational complexity in lookup, insertion, deletion of entries, but it greatly simplifies the development process: it allows to easily translate the pseudo-code into the actual C++ implementation. Real world implementations use data structures such as linked lists or arrays of nodes.
2. Due to the way the `Trail` class is implemented, in many cases I decided to copy the whole instance instead of making in place modifications. A smarter implementation of the `Trail` class could also simplify the hamiltonicity check to a $O(n)$ instead of the current $O(n^2)$.
3. I did not take into account space complexity: when LK was published it was a problem, and instances with sizes above 100 were a problem. As of now, storage utilization is certainly not the first concern. A possible solution, that would reduce storage utilization, while not increasing the complexity of the implementation, is to use persistent data structures. Persistent data structures would allow having multiple versions of the same data structure, without having copies of such data structures. C++ offers persistent data structures only through third party libraries.
4. Optimizations from the original LK paper are missing, \S 2.A, \S 2.B, \S 2.C, \S 2.D in [@lin_effective_1973].

# MILP TSP

## Model

This model is described more accurately in [@gavish_travelling_1978].

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

## Implementation

> Note that every `NULL` field is automatically filled by a default value by the CPLEX API.

Listing: Initialization of variable (7), from [MILP model].

```{#lst:initYVars .cpp}
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

Listing: Initialization of variable (6), from [MILP model].

```{#lst:initXVars .cpp}
for(int i = 0; i < N * (N - 1); ++i) {
    CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, NULL, NULL, NULL, NULL, NULL);
}
```

Listings \ref{lst:initYVars} and \ref{lst:initXVars} contain the initialization of the variables, (6) and (7) from the model, respectively.
Note that:

 - each $y_{i,j}$ variable is coupled with its weight, $c_{i,j}$, the weight $c_{i,i}$ is replaced by an infinite value, that is because otherwise the model considers the edge $(i,i)$ while building the hamiltonian cycle ($c_{i,i} = 0$, since the distance between node $i$ and node $i$ is $0$)
 - constraint (6) describes $x_{i,j}$ as a positive, real, number coupled with each edge in $A$, except for $(i,j), j = 0$, thus I initialized $N * (N - 1)$ variables, which are variables $x_{0,1}, x_{0,2}, ..., x_{1,1},..., x_{|N| - 1, |N| - 1}$
 - the default values for type and bounds of a variable create a continuous number, bounded between zero and infinite.

Listing: Creation of the first constraint, (2) in [MILP model].

```{#lst:firstConstraint .cpp}
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
  CHECKED_CPX_CALL(
    CPXaddrows, 
    env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL
  );
}
```
Listing: Constraint (3) from [MILP model].

```{#lst:secondConstraint .cpp}
for(int i = 0; i < N; ++i) {
  std::vector<int> idx(N);
  for(int j = 0; j < N; ++j) { // y_i_j 
    idx[j] = i * N + j;
  }
  std::vector<double> coef(N, 1.0);
  double rhs = 1.0;
  int matbeg = 0;
  CHECKED_CPX_CALL(
    CPXaddrows, 
    env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL
  );
}
```

Listing: Constraint (4) from [MILP model].

```{#lst:thirdConstraint .cpp}
for(int j = 0; j < N; ++j) {
  std::vector<int> idx(N);
  for(int i = 0; i < N; ++i) { // y_i_j 
    idx[i] = i * N + j;
  }
  std::vector<double> coef(N, 1.0);
  double rhs = 1.0;
  int matbeg = 0;
  CHECKED_CPX_CALL(
    CPXaddrows, 
    env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL
  );
}
```

Listing: Constraint (5) from [MILP model].

```{#lst:fourthConstraint .cpp}
for (int i = 0; i < N; ++i) {
  for (int j = 1; j < N; ++j) {
    std::vector<int> idx(2);
    idx[0] = i * N + j; // y_i_j
    idx[1] = N * N + i * (N - 1) + j - 1; // x_i_j
    std::vector<double> coef(2, 1.0);
    coef[0] = - N + 1.0;
    char sense = 'L';
    int matbeg = 0;
    CHECKED_CPX_CALL(
      CPXaddrows, 
      env, lp, 0, 1, idx.size(), NULL, &sense, &matbeg, &idx[0], &coef[0], NULL, NULL
    );
  }
}
```

# Tests

## Test data

Through the instance generator I generated 5 different types of instances:

 - 10 nodes, size of two-dimensional plane $10\times 10$cm, the distance between nodes in the grid is $1,5$cm
 - 20 nodes, $10\times 15$cm, $1,5$cm
 - 40 nodes, $20\times 15$cm, $1,5$cm
 - 80 nodes, $30\times 20$cm, $2,0$cm
 - 100 nodes, $30\times 25$cm, $2,0$cm.

There are 10 randomly generated instances of each size.
LK is executed 10 times for each instance, and data is gathered in order to compute
the best cost, the worst, and the difference with the exact solution.

## Test results

> The benchmark suite was executed on a laptop equipped with linux, with 8gb of ram, a 6 cores, 12 threads CPU, a local CPLEX install.

\ref{tab:results} shows the costs, and time for each instance, and
\ref{tab:summary} is a summary, grouped by size of the instance.
The character `-` in both tables represents either a $0$ or a value smaller than $0,00001$.


| Instance | Best  solution | Exact solution | LK CPU time (s) | MILP user time (s) | MILP CPU time (s) |
| --- | --- | --- | --- | --- | --- |
| 010_s_0 | 25.167 | 25.167 | 0,00135 | 0,04076 | 0,14415 |
| 010_s_1 | 22.416 | 22.416 | 0,00075 | 0,03748 | 0,15791 |
| 010_s_2 | 29.489 | 29.489 | 0,00051 | 0,03324 | 0,17605 |
| 010_s_3 | 22.950 | 22.950 | 0,00063 | 0,03500 | 0,17805 |
| 010_s_4 | 25.339 | 25.339 | 0,00038 | 0,05050 | 0,19526 |
| 010_s_5 | 27.524 | 27.524 | 0,00091 | 0,02911 | 0,11154 |
| 010_s_6 | 29.547 | 29.547 | 0,00157 | 0,05778 | 0,34336 |
| 010_s_7 | 26.314 | 26.314 | 0,00046 | 0,03196 | 0,13062 |
| 010_s_8 | 23.121 | 23.121 | 0,00076 | 0,05941 | 0,25044 |
| 010_s_9 | 29.610 | 29.610 | 0,00121 | 0,03637 | 0,17019 |
| 020_sm_0 | 48.693 | 48.693 | 0,00256 | 0,21610 | 1,78815 |
| 020_sm_1 | 53.658 | 53.658 | 0,01005 | 0,23730 | 1,76421 |
| 020_sm_2 | 45.384 | 45.384 | 0,00436 | 0,10709 | 0,64581 |
| 020_sm_3 | 49.323 | 49.323 | 0,00550 | 0,41923 | 3,87906 |
| 020_sm_4 | 44.677 | 44.677 | 0,00369 | 0,25804 | 2,02248 |
| 020_sm_5 | 42.289 | 42.289 | 0,00271 | 0,20588 | 1,41190 |
| 020_sm_6 | 51.353 | 51.353 | 0,00610 | 0,24259 | 1,78205 |
| 020_sm_7 | 45.288 | 45.288 | 0,00739 | 0,29124 | 2,49672 |
| 020_sm_8 | 45.257 | 45.257 | 0,01005 | 0,23576 | 1,97495 |
| 020_sm_9 | 48.117 | 48.117 | 0,00319 | 0,16028 | 0,93129 |
| 040_m_0 | 92.802 | 92.802 | 0,04897 | 2,82971 | 26,82781 |
| 040_m_1 | 91.263 | 91.263 | 0,05195 | 1,84738 | 15,95464 |
| 040_m_2 | 90.792 | 90.792 | 0,08687 | 0,71296 | 4,77066 |
| 040_m_3 | 91.971 | 91.971 | 0,02145 | 2,10037 | 20,16199 |
| 040_m_4 | 94.764 | 94.764 | 0,07614 | 2,56792 | 21,79439 |
| 040_m_5 | 95.021 | 94.143 | 0,04539 | 1,16249 | 8,75681 |
| 040_m_6 | 92.821 | 91.063 | 0,04020 | 2,38876 | 21,92190 |
| 040_m_7 | 101.209 | 101.150 | 0,03608 | 3,64844 | 35,97391 |
| 040_m_8 | 81.903 | 81.903 | 0,02212 | 0,87161 | 7,01236 |
| 040_m_9 | 92.821 | 92.821 | 0,04667 | 3,12038 | 29,80253 |
| 080_ml_0 | 195.848 | 192.792 | 0,30637 | 16,90118 | 108,46413 |
| 080_ml_1 | 193.021 | 192.077 | 0,31063 | 63,48429 | 561,80182 |
| 080_ml_2 | 187.872 | 187.872 | 0,20446 | 27,11544 | 187,47461 |
| 080_ml_3 | 188.052 | 187.696 | 0,20163 | 20,61190 | 129,28365 |
| 080_ml_4 | 184.996 | 184.996 | 0,20483 | 34,35254 | 305,59034 |
| 080_ml_5 | 191.032 | 190.548 | 0,20869 | 36,67534 | 326,07649 |
| 080_ml_6 | 189.020 | 188.204 | 0,18789 | 31,12883 | 256,97143 |
| 080_ml_7 | 194.320 | 193.148 | 0,21281 | 86,12654 | 831,54008 |
| 080_ml_8 | 192.792 | 189.504 | 0,28244 | 25,41119 | 167,63328 |
| 080_ml_9 | 191.008 | 190.652 | 0,41513 | 25,52500 | 211,01538 |
| 100_l_0 | 243.416 | 240.460 | 0,57485 | 292,45697 | 2.880,82138 |
| 100_l_1 | 237.989 | 236.104 | 0,50675 | 168,22690 | 1.554,66110 |
| 100_l_2 | 233.252 | 231.468 | 0,56689 | 186,77969 | 1.658,41696 |
| 100_l_3 | 235.492 | 234.192 | 0,42911 | 68,61825 | 432,92631 |
| 100_l_4 | 233.608 | 232.436 | 0,46774 | 97,99636 | 898,65973 |
| 100_l_5 | 231.976 | 229.836 | 0,58532 | 171,34566 | 1.568,17337 |
| 100_l_6 | 232.460 | 230.344 | 0,38482 | 314,79868 | 3.189,95584 |
| 100_l_7 | 235.364 | 232.536 | 0,55136 | 68,79076 | 586,30417 |
| 100_l_8 | 236.460 | 233.044 | 0,38117 | 184,59165 | 1.660,02422 |
| 100_l_9 | 236.204 | 234.676 | 0,41010 | 51,99832 | 415,53863 |


: The table displays the min, max, exact cost obtained by running the algorithm Lin-Kernighan 10 times for each instance.contains data about running times, in seconds.\label{tab:results}

| Nodes | Mean cpu time | Mean error (%) | Max error (%) | LK std dev CPU time (%) |
| --: | --: | ---: | ---: | ----: |
| 10 | 0,00085 | - | - | 47,18% |
| 20 | 0,00556 | - | - | 50,77% |
| 40 | 0,04758 | 0,29% | 1,93% | 43,77% |
| 80 | 0,25349 | 0,55% | 1,74% | 28,95% |
| 100 | 0,48581 | 0,90% | 1,47% | 16,75% |

: Running times  and results of the instances tested, grouped by size of instance, using the Lin-Kernighan heuristic implementation. The table displays the mean and max relative error, with respect to the exact solutions obtained via MILP, the mean CPU time, in seconds, and the relative standard deviation of the CPU time in LK. \label{tab:summary}

From \ref{tab:summary} I notice the following:

 - running times vary considerably within instances of the same size, that is also something that is apparent in MILP, but this time I also notice that execution times vary between different executions of the same instance, that is because each time LK starts with a different random tour, which is shown in \ref{img:stdcost}
 - errors are on average less than $1%$, and less than $2%$ in the worst case, which is surprisingly good.

![A graph that represents the relative standard deviation of cost, grouped by number of nodes.\label{img:stdcost}](src/relStdDev.pdf)

Graphs \ref{img:time}, \label{img:timelog}, \label{img:timecpuuser} all show different visualization for execution
time of MILP against LK. I already know from table \ref{tab:results} that on average, even on the biggest 
instance, LK improves by a huge margin execution times. The visualizations plot time complexity and how it behaves with respect of the different instances, from the trend lines I can immediately notice that MILP grows much faster than LK, especially towards the end. 

![A graph that represents the behavior of LK with respect to CPU time, in seconds, grouped by number of nodes.\label{img:time}](src/cpuTimeNodes.pdf)

![A graph that represents the CPU time of both LK and MILP implementation, in logarithmic scale, grouped by nodes.\label{img:timelog}](src/cpuTimeNodesMerge.pdf)

![A graph that represents the CPU time of LK and user time of MILP, in seconds, grouped by number of nodes.\label{img:timecpuuser}](src/cpuAndUserTimeNodesMerge.pdf)

# Conclusion

In this report I showed a surprisingly good heuristic, LK, and how it performs
against an algorithm which finds the exact solution, namely MILP. These two 
approaches are then compared with respect to the solution and the execution time.
I notice that LK always provides LK solutions with near optimal cost with a margin
of error which is consistently under 2%, at least for instances with less than 100 holes. LK execution times are considerably lower than MILP. If I would have to answer the question of whether if to use MILP or LK in a 
TSP drilling problem, I would chose LK.

Note that in this report the biggest graph has 100 nodes, there might 
be electronic boards that need more than 100 holes. LK shows promising result with
the following instance, <https://www.math.uwaterloo.ca/tsp/vlsi/pbm436.tour.html>,
even though execution times grow considerably, to around 15 minutes, the best solution
found is still near optimal, 1443 against 1471.

# References
