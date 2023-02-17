#ifndef GRAPH_H
#define GRAPH_H

#include<vector>

struct Graph {

  // number of edges
  const int N;

  // weights of the edges c(i, j) <==> c[i][j]
  std::vector<std::vector<double>> c;

  Graph(const int _N, const std::vector<std::vector<double>>& _c): N(_N), c(_c) {};

};

#endif /* GRAPH_H */
