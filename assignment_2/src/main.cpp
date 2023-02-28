#include "parser.h"
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

struct Results {
  double result;
  double userTime;
  double cpuTime;
};

const Results runLK(const std::unique_ptr<LinKernighan>& lk, const Graph& g) {
  clock_t t1,t2;
  t1 = clock();
  struct timeval  tv1, tv2;
  gettimeofday(&tv1, NULL);
  const auto& tour = lk->localOptimalTour(g);          
  t2 = clock();
  gettimeofday(&tv2, NULL);

  double tourCost = tour->cost(g);
  
  return {
    tourCost,
    (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)),
    (double)(t2-t1) / CLOCKS_PER_SEC
  };

}

int main(int argc, char const *argv[]) {

  try {
    
    const auto parser = std::make_unique<const Parser>();
    const auto linKernighan = std::make_unique<LinKernighan>();

    if (argc >= 2) {
      const auto graph = parser->buildGraph("../tsp_instances/" + std::string(argv[1]));
      std::cout<< "Parsed graph of size " << graph.N << "\n";
      
      const auto results = runLK(linKernighan, graph);

      std::cout << "Tour cost: " << results.result << "\n"
                << "User time (seconds): " <<  results.userTime << "\n"
                << "CPU time (seconds): " << results.cpuTime << "\n";

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
        [&myfile, &linKernighan](auto &tuple){ 

          auto& [filename, g] = tuple;
          std::cout << filename << " TSP value: ";

          const auto results = runLK(linKernighan, *g.get());
          std::cout << results.result << "\n";

          myfile 
            << filename << ","
            << g->N << ","
            << g->N * g->N << ","
            << results.result << ","
            << results.userTime << ","
            << results.cpuTime << "\n";
        }
      
      );

      myfile.close();
    }
  } catch (std::exception& e) {
		
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}

  return 0;
}
