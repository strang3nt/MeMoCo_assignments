#include "model.h"
#include "parser.h"
#include <memory>
#include <iostream>

// error  status and message buffer
int status;
char errmsg[BUF_SIZE];

int main(int, char**) {

  try {
    DECL_ENV(env);

    const auto parser = std::make_unique<Parser>();
    const auto model = std::make_unique<Model>(env);

    const auto graph = parser->buildGraph("../tsp_instances/input_float_90_24.txt");
    std::cout<< "Parsed graph of size " << graph.N << "\n";
    const auto timings = model->solveTsp(graph);

    // free
    CPXcloseCPLEX(&env);

    std::cout << "CPU time is: " << timings.cpuTime << "\n"
              << "user time is: " << timings.userTime << "\n";
  
  } catch (std::exception& e) {
		
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}
  return 0;
}
