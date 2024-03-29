#include "parser.h"
#include "graph.h"

#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <limits>

std::vector<double> Parser::stringToArray(const std::string &s, const int words) {
  std::vector<double> edge(words);
  std::stringstream ssin(s);
  for (int i = 0; i < words && ssin.good(); i++ ) {
      ssin >> edge[i];
  }
  return edge;
}

std::vector<std::string> Parser::file(const std::string& path) {

  std::vector<std::string> lines;
  std::string line;
  std::ifstream input_file(path);
  
  getline(input_file, line);
  auto header = (int) stringToArray(line, 1)[0];

  for(int i = 0; i < header; i++) {
    getline(input_file, line);
    lines.push_back(line);
  }

  input_file.close();
  return lines;
}

Graph Parser::buildGraph(const std::string& path) {

    auto edgesString = file(path);
    const int N = edgesString.size();
    std::vector<std::vector<double>> nodes;

    for(int i = 0; i < N; i++) {
      nodes.push_back(stringToArray(edgesString[i], 3));
    }

    std::vector<std::vector<int>> c(N, std::vector<int>(N, 0));
    
    for(int i = 0; i < N; i++) {
      for(int j = 0; j < N; j++) {
        double x1 = nodes[i][1];
        double y1 = nodes[i][2];
        double x2 = nodes[j][1];
        double y2 = nodes[j][2];

        // euclidean distance
        c[i][j] = j == i ? 
          std::numeric_limits<int>::max() : 
          std::round(std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2)));      
      }
    }

    return Graph(N, c);
}
