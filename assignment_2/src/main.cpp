#include "parser.h"
#include "model.h"
#include "linKernighan.h"

#include <ctime>
#include <sys/time.h>
#include <memory>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <execution>
#include <tuple>

// error  status and message buffer
int status;
char errmsg[BUF_SIZE];

int main(int argc, char const *argv[]) {

  try {
    
    const auto parser = std::make_unique<const Parser>();
    DECL_ENV(env);
    const auto model = std::make_unique<const Model>(env);
    const auto linKernighan = std::make_unique<LinKernighan>();

    if (argc >= 2) {
      const auto graph = parser->buildGraph("../tsp_instances/" + std::string(argv[1]));
      std::cout<< "Parsed graph of size " << graph.N << "\n";
      
      clock_t t1,t2;
      t1 = clock();
      struct timeval  tv1, tv2;
      gettimeofday(&tv1, NULL);

      const auto feasibleTour = model->getFeasibleSolution(graph.N, graph.c, 1);
      const auto& tour = linKernighan->localOptimalTour(feasibleTour, graph.c);
          
      t2 = clock();
      gettimeofday(&tv2, NULL);

      double tourCost = 0;
      for(const auto& [i, j]: tour) {
        tourCost += graph.c[i][j];
      }

      std::cout << "Tour cost: " << tourCost << "\n"
                << "User time (seconds): " << (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)) << "\n"
                << "CPU time (seconds): " << (double)(t2-t1) / CLOCKS_PER_SEC << "\n";

    } else {

      auto path = std::filesystem::path("../tsp_instances");
      std::cout << "Parsing files from directory " << std::filesystem::canonical(path) << "\n";
      auto files = std::filesystem::directory_iterator(path);

      int fileCount = std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator{});
      std::vector<int> a(fileCount);
      std::iota(a.begin(), a.end(), 0);
      std::vector<std::tuple<std::string, std::unique_ptr<Graph>>> graphs(fileCount);

      std::transform(
        std::execution::par,
        std::filesystem::begin(files),
        std::filesystem::end(files),
        a.begin(),
        graphs.begin(),
        [&graphs, &parser](const auto &f, const auto &i){ 
          return std::make_tuple(
            f.path().stem().string(), 
            std::make_unique<Graph>(parser->buildGraph(f.path()))
          ); 
        });

      std::cout<< "Parsed " << graphs.size() << " files.\n";

      std::sort(graphs.begin(), graphs.end());

      std::ofstream myfile;
      myfile.open ("tsp_results.csv");
      myfile << "Instance,Nodes,Edges,Result,User time,CPU time\n";

      std::for_each(
        graphs.begin(),
        graphs.end(),
        [&myfile, &model, &env, &linKernighan](auto &tuple){ 

          auto& [filename, g] = tuple;
          std::cout << filename << " TSP value: ";

          clock_t t1,t2;
          t1 = clock();
          
          struct timeval  tv1, tv2;
          gettimeofday(&tv1, NULL);

          const auto feasibleTour = model->getFeasibleSolution(g->N, g->c, 1);
          const auto& tour = linKernighan->localOptimalTour(feasibleTour, g->c);
          
          t2 = clock();
          gettimeofday(&tv2, NULL);

          double tourCost = 0;
          for(const auto& [i, j]: tour) {
            tourCost += g->c[i][j];
          }
          std::cout << tourCost << "\n";

          myfile 
            << filename << ","
            << g->N << ","
            << g->N * g->N << ","
            << tourCost << ","
            << (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)) << ","
            << (double)(t2-t1) / CLOCKS_PER_SEC << "\n";
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
