#include "parser.h"
#include "model.h"

#include <ctime>
#include <sys/time.h>
#include <memory>
#include <iostream>
#include <string>

// error  status and message buffer
int status;
char errmsg[BUF_SIZE];

int main(int argc, char const *argv[]) {

  try {
    
    if (argc < 2) throw std::runtime_error("usage: ./main filename.dat");

    const auto parser = std::make_unique<Parser>();
    const auto graph = parser->buildGraph("../tsp_instances/" + std::string(argv[1]));
    std::cout<< "Parsed graph of size " << graph.N << "\n";

    DECL_ENV(env);
    // CHECKED_CPX_CALL(CPXsetintparam, env, CPX_PARAM_DATACHECK, CPX_ON);
    DECL_PROB(env, lp);

    const auto model = std::make_unique<Model>(env, lp);
    model->initTsp(graph.N, graph.c);
    const auto timings = model->solveTsp(graph.N);

    // free
    CPXfreeprob(env, &lp);
    CPXcloseCPLEX(&env);

    std::cout << "User time (seconds): " << timings.userTime << "\n"
              << "CPU time (seconds): " << timings.cpuTime << "\n";

  } catch (std::exception& e) {
		
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}
  return 0;
}
