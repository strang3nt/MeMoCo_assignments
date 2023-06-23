#include "parser.h"
#include "model.h"
#include "cpxmacro.h"

#include <ctime>
#include <sys/time.h>
#include <memory>
#include <iostream>
#include <string>
#include <experimental/filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <iomanip>

namespace fs = std::experimental::filesystem;

// error  status and message buffer
int status;
char errmsg[BUF_SIZE];

int main(int argc, char const *argv[]) {

  std::cout.precision(std::numeric_limits<double>::max_digits10 + 1);
  try {
    
    const auto parser = std::make_unique<Parser>();
    
    DECL_ENV(env);
    CPXsetdblparam (env, CPXPARAM_TimeLimit, 1000);
    const auto model = std::make_unique<Model>(env);

    if (argc >= 2) {
      const auto graph = parser->buildGraph("../tsp_instances/" + std::string(argv[1]));
      std::cout<< "Parsed graph of size " << graph.N << "\n";
      const auto results = model->solveTsp(graph.N, graph.c);

      std::cout << "Result: " << results.result << "\n"
                << "User time (seconds): " << results.userTime << "\n"
                << "CPU time (seconds): " << results.cpuTime << "\n";
    } else {

      auto path = fs::path("../tsp_instances");
      std::cout << "Parsing files from directory " << fs::canonical(path) << "\n";
      auto files = fs::directory_iterator(path);

      int fileCount = std::distance(fs::directory_iterator(path), fs::directory_iterator{});
      std::vector<int> a(fileCount);
      std::iota(a.begin(), a.end(), 0);
      std::vector<std::tuple<std::string, std::unique_ptr<Graph>>> graphs(fileCount);

      std::transform(
        fs::begin(files),
        fs::end(files),
        a.begin(),
        graphs.begin(),
        [&graphs, &parser](const auto &f, const auto &i){ 
          return std::make_tuple(
            f.path().stem().string(), 
            std::make_unique<Graph>(parser->buildGraph(f.path()))
          ); 
        });

      std::sort(graphs.begin(), graphs.end());

      std::ofstream myfile;
      myfile.open ("tsp_results.csv");
      myfile << "instance,nodes,result,userTime,cpuTime\n";

      std::for_each(
        graphs.begin(),
        graphs.end(),
        [&myfile, &model, &env](auto &tuple){ 

          auto& [filename, g] = tuple;
          std::cout << filename << " TSP value: ";
          const auto results = model->solveTsp(g->N, g->c);
          std::cout << results.result << "\n";
          myfile << std::setprecision(10)
            << filename << ","
            << g->N << ","
            << results.result << ","
            << results.userTime << ","
            << results.cpuTime << "\n";
        }
      );

      myfile.close();
    }

    // free
    CPXcloseCPLEX(&env);
  } catch (std::exception& e) {
		
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}

  return 0;
}
