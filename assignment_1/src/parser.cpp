#include "parser.h"
#include "graph.h"
#include<vector>
#include<fstream>
#include<string>
#include<algorithm>
#include<sstream>
#include<cmath>
#include<iostream>

std::vector<double> Parser::stringToArray(const std::string &s, int words) {
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
  
  while (getline(input_file, line)) {
    lines.push_back(line);
  }

  input_file.close();
  return lines;
}

Graph Parser::buildGraph(const std::string& path) {

    auto edgesString = file(path);

    const int N = stringToArray(edgesString[0], 1)[0];
    edgesString.erase(edgesString.begin());

    std::vector<std::vector<double>> nodes;
    for(int i = 0; i < N; i++) {
      nodes.push_back(stringToArray(edgesString[i], 2));
    }

    std::vector<std::vector<double>> c(N);
    
    for(int i = 0; i < N; i++) {
      for(int j = 0; j < N; j++) {
        double x1 = nodes[j][0];
        double y1 = nodes[j][1];
        double x2 = nodes[i][0];
        double y2 = nodes[i][1];
        double distance = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2-y1, 2));
        c[i].push_back(distance);

      }
    }

    return Graph(N, c);
}