#include "parser.h"
#include "graph.h"
#include "model.h"
#include <memory>
#include <iostream>

// error  status and message buffer
int status;
char errmsg[BUF_SIZE];

int main(int, char**) {

  std::cout<< "Hello";
  const auto parser = std::make_unique<Parser>();
  const auto model = std::make_unique<Model>();

  std::cout<< "Starting parse";
  const auto graph = parser->buildGraph("../../tsp_instances/input_float_9_4.txt");
  std::cout<< "graph parsed\n";
  const auto timings = model->solveTsp(graph);

  std::cout << "CPU time is: " << timings.cpuTime << "\n"
            << "user time is: " << timings.userTime << "\n";
  
  return 0;
}