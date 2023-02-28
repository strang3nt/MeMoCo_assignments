#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "graph.h"

class Parser {

  private:

    std::vector<double> stringToArray(const std::string&, const int);
    std::vector<std::string> file(const std::string&);

  public:
    Graph buildGraph(const std::string&);

};

#endif /* PARSER_H */