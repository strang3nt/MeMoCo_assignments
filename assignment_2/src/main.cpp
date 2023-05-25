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
#include <tuple>
#include <cmath>
#include <iomanip>

struct Result {
  int result;
  double userTime;
  double cpuTime;
};

double mean(const std::vector<double>& data) {
  double sum = std::accumulate(data.begin(), data.end(), 0.0);
  return sum / data.size();
}

double mean(const std::vector<int>& data) {
  int sum = std::accumulate(data.begin(), data.end(), 0);
  return sum / static_cast<double>(data.size());
}

template <typename T>
double stdDev(double mean, const std::vector<T>& data) {
  double stdDev = 0.0;
  int n = data.size();
  for(int i = 0; i < n; ++i) {
    stdDev += std::pow(data[i] - mean, 2);
  }

  return std::sqrt(stdDev / n);
}

const Result runLK(const std::unique_ptr<LinKernighan>& lk, const Graph& g) {
  clock_t t1,t2;
  t1 = clock();
  struct timeval  tv1, tv2;
  gettimeofday(&tv1, NULL);
  const auto& tour = lk->localOptimalTour(g);          
  t2 = clock();
  gettimeofday(&tv2, NULL);
  return {
    tour->cost(g),
    (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)),
    (double)(t2-t1) / CLOCKS_PER_SEC
  };
}

int main(int argc, char const *argv[]) {

  std::cout.precision(std::numeric_limits<double>::max_digits10 + 1);

  try {
    
    const auto parser = std::make_unique<Parser>();
    const auto linKernighan = std::make_unique<LinKernighan>();

    if (argc >= 2) {
      const auto &graph = parser->buildGraph("../tsp_instances/" + std::string(argv[1]));
      std::cout<< "Parsed graph of size " << graph.N << "\n";
      
      const auto &results = runLK(linKernighan, graph);

      std::cout << "Tour cost: " << results.result << "\n"
                << "User time (seconds): " <<  results.userTime << "\n"
                << "CPU time (seconds): " << results.cpuTime << "\n";

    } else {

      auto path = std::filesystem::path("../tsp_instances");
      std::cout << "Parsing files from directory " << std::filesystem::canonical(path) << "\n";
      auto files = std::filesystem::directory_iterator(path);
      std::vector<std::tuple<std::string, std::unique_ptr<Graph>>> graphs;

      std::transform(
        std::filesystem::begin(files),
        std::filesystem::end(files),
        std::back_inserter(graphs),
        [&parser](const auto &f){ 
          return std::make_tuple(
            f.path().stem().string(), 
            std::make_unique<Graph>(parser->buildGraph(f.path()))
          ); 
        });

      std::cout<< "Parsed " << graphs.size() << " files.\n";

      std::sort(graphs.begin(), graphs.end());
	  
      std::ofstream myfile;
      myfile.open ("tsp_results.csv");
      myfile << "instance,nodes,minCost,maxCost,meanCost,stdDevCost,minCpuTime,maxCpuTime,meanCpuTime,stdDevCpuTime\n";

      std::for_each(
        graphs.cbegin(),
        graphs.cend(),
        [&myfile, &linKernighan](const auto& tuple){

          const auto& [filename, g] = tuple;
          
          std::vector<int> costs;
          std::vector<double> cpuTimes;
          
          for(int i = 0; i < 10; ++i) {
            const auto result = runLK(linKernighan, *g.get());
            costs.push_back(result.result);
            cpuTimes.push_back(result.cpuTime);
          }
        
          auto costsMean = mean(costs);
          auto costsStdDev = stdDev(costsMean, costs);

          auto cpuTimesMean = mean(cpuTimes);
          auto cpuTimesStdDev = stdDev(cpuTimesMean, cpuTimes);

          auto minCost = std::min_element(costs.begin(), costs.end());
          auto maxCost = std::max_element(costs.begin(), costs.end());
          auto minCpuTime = std::min_element(cpuTimes.begin(), cpuTimes.end());
          auto maxCpuTime = std::max_element(cpuTimes.begin(), cpuTimes.end());

          /**
           * Instance,
           * Nodes,
           * Min cost,
           * Max cost,
           * Mean cost,
           * Std dev cost,
           * Min CPU time,
           * Max CPU time,
           * Mean CPU time,
           * Std dev CPU time
          */
          std::cout << filename << " TSP value: " << *minCost << "\n";
          myfile << std::setprecision(10)
            << filename << ","
            << g.N << ","
            << *minCost << ","
            << *maxCost << ","
            << costsMean << ","
            << costsStdDev << ","
            << *minCpuTime << ","
            << *maxCpuTime << ","
            << cpuTimesMean << ","
            << cpuTimesStdDev << "\n";
        }
      
      );

      myfile.close();
    }
  } catch (std::exception& e) {
		
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}

  return 0;
}
