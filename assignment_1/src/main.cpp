#include "parser.h"
#include "model.h"

#include <ctime>
#include <sys/time.h>
#include <memory>
#include <iostream>

// error  status and message buffer
int status;
char errmsg[BUF_SIZE];

int main(int, char**) {

  try {
    
    const auto parser = std::make_unique<Parser>();
    const auto graph = parser->buildGraph("../tsp_instances/input_float_9_4.txt");
    std::cout<< "Parsed graph of size " << graph.N << "\n";

    DECL_ENV(env);
    DECL_PROB(env, lp);

    const auto model = std::make_unique<Model>(env, lp);
    model->initTsp(graph.N, graph.c);
    const auto timings = model->solveTsp(graph.N);

    // free
    CPXfreeprob(env, &lp);
    CPXcloseCPLEX(&env);

    std::cout << "CPU time: " << timings.cpuTime << "\n"
              << "User time: " << timings.userTime << "\n";

  } catch (std::exception& e) {
		
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}
  return 0;
}
