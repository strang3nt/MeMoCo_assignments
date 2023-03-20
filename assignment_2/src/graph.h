#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <algorithm>
#include <numeric>
#include <tuple>

struct Graph {

  // number of edges
  const int N;

  // weights of the edges c(i, j) <==> c[i][j]
  std::vector<std::vector<double>> c;

  Graph(const int _N, const std::vector<std::vector<double>>& _c): N(_N), c(_c) {};

  const std::vector<int> getNearestNeighbors(const int node) const {
      
    const size_t n = this->c.at(node).size();
    std::vector<double> costs(this->c.at(node));
    std::vector<int> indices(n);
    std::iota(indices.begin(), indices.end(), 0);
    std::vector<std::tuple<double, int>> costsIndices(n);
    std::transform(
      costs.begin(), costs.end(),
      indices.begin(),
      costsIndices.begin(), [] (const auto& cost, const auto& index) {
        return std::make_tuple(cost, index);
      }
    );
    std::sort(costsIndices.begin(), costsIndices.end());
    std::vector<int> orderedNeighbors(n);
    std::transform(
      costsIndices.begin(),
      costsIndices.end(),
      orderedNeighbors.begin(), [] (const auto& costIndex) { return std::get<1>(costIndex); }
    );
    return orderedNeighbors;
  };

};

#endif /* GRAPH_H */
