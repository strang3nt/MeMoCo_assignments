#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "graph.h"

class Parser {

  private:

    std::vector<double> stringToArray(const std::string&, int) const;
    std::vector<std::string> file(const std::string&) const;

  public:
    Graph buildGraph(std::string) const;

};

#endif /* PARSER_H */