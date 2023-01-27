#ifndef GRAPH_H
#define GRAPH_H

#include<memory>
#include<vector>
#include<list>
#include<tuple>

struct Graph {

    // number of edges
    const int N;

    // weights of the edges c(i, j) <==> c[i][j]
    const std::vector<std::vector<double>> c;

    Graph(const int N, const std::vector<std::vector<double>>& c): N(N), c(c) {};

};

#endif /* GRAPH_H */