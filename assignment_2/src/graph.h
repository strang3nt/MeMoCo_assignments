#ifndef GRAPH_H
#define GRAPH_H

#include<vector>
#include "cpxmacro.h"
#include <algorithm>
#include <tuple>
#include <iterator>
#include <numeric>

struct Graph {

  // number of edges
  const int N;

  // weights of the edges c(i, j) <==> c[i][j]
  std::vector<std::vector<double>> c;

  Graph(const int _N, const std::vector<std::vector<double>>& _c): N(_N), c(_c) {};

  std::vector<std::vector<std::tuple<double, int>>> getOrderedAdjacencyList() {
    auto adjList = std::vector<std::vector<std::tuple<double, int>>>(N, std::vector<std::tuple<double,int>>(N, std::make_tuple<double, int>(CPX_INFBOUND, -1)));

    for (int i = 0; i < N; i++) {
      std::vector<double> row;
      std::copy(c[i].begin(), c[i].end(), std::back_inserter(row));
      std::vector<int> indices(N);
      std::iota(indices.begin(), indices.end(), 0);

      std::vector<std::tuple<double, int>> costs_indices(N, std::make_tuple(CPX_INFBOUND, -1));

      std::transform(
        row.begin(), row.end(), indices.begin(), costs_indices.begin(),
        [](const auto& r, const auto& i) { return std::make_tuple(r, i); }
      );

      std::stable_sort(costs_indices.begin(), costs_indices.begin());
      adjList[i] = costs_indices;
    }

    return adjList;
  }

};

#endif /* GRAPH_H */