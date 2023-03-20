#ifndef LINKERNIGHAN_H
#define LINKERNIGHAN_H

#include <vector>
#include <tuple>
#include <deque>
#include <algorithm>
#include <iterator>
#include <set>
#include <memory>

#include "trail.h"

class LinKernighan {

  public:
  
  const std::unique_ptr<Trail> localOptimalTour(const Graph& g) {

    const auto& weight = g.c;
    auto tour = Trail::getRandomTour(weight);
    int p2 = 2;
    int p1 = 5;

    std::deque<std::tuple<int, int, double>> stack;
    std::vector<int> alternatingTrail(weight.size() * 2, -1);
    std::unique_ptr<Trail> F;
    double G = 0.0;

    do {

      G = 0.0;
      F = std::make_unique<Trail>();
      for(size_t u = 0; u < weight.size(); ++u) { stack.push_front({u, 0, 0.0}); }
      
      while (!stack.empty()) {
        const auto [vi, i, gi] = stack.front();
        stack.pop_front();
        alternatingTrail.at(i) = vi;
        const auto& alternatingTrailPairwise = std::make_unique<Trail>(alternatingTrail, i);

        if (i % 2 == 0) {

          const auto& diff = tour->trailDifference(*alternatingTrailPairwise.get());
          for(size_t u = 0; u < weight.size(); ++u) {

            const auto& xi = Trail::makeTuple(vi, u);
            const auto& yi = Trail::makeTuple(u, alternatingTrail.at(0));

            if(diff->contains(xi)) {
              
              alternatingTrailPairwise->insert(xi);
              const auto& union_ = tour->trailUnion(*alternatingTrailPairwise.get());
              alternatingTrailPairwise->insert(yi);
              const auto& symDiff = tour->trailSymmetricDifference(*alternatingTrailPairwise.get());

              if(i <= p2 || (!union_->contains(yi) && symDiff->isTour(g.N))) {
                stack.push_front({u, i + 1, gi + weight.at(vi).at(u)});
              }
              alternatingTrailPairwise->erase(xi);
              alternatingTrailPairwise->erase(yi);

            }
          }
        }

        else {

          double candidateG = gi - weight.at(vi).at(alternatingTrail.at(0));
          const auto& currTrail = tour->trailUnion(*alternatingTrailPairwise.get());
          const auto& endNode = Trail::makeTuple(vi, alternatingTrail.at(0));
          alternatingTrailPairwise->insert(endNode);
          const auto& symDiff = tour->trailSymmetricDifference(*alternatingTrailPairwise.get());
          
          if (candidateG > 0 && candidateG > G && symDiff->isTour(g.N)) {
            F = std::make_unique<Trail>(*alternatingTrailPairwise.get()); // copy constructor
            G = candidateG;
          }

          const auto neighbors = g.getNearestNeighbors(vi);
          for(auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
            if(gi > weight.at(vi).at(*it) &&
              !currTrail->contains(Trail::makeTuple(vi, *it)))
                stack.push_front({*it, i + 1, gi - weight.at(vi).at(*it)});
          }
          alternatingTrailPairwise->erase(endNode);
        }
        
        int j = 0;
        std::tie(std::ignore, j, std::ignore) = stack.front();
        if(i <= j) {
          if(G > 0) {
            tour = tour->trailSymmetricDifference(*F.get());
            stack.clear();
          } else if(i > p1){
            while(!stack.empty() && std::get<1>(stack.front()) > p1) {
              stack.pop_front();
            }
          }
        }
      } 

    } while (G > 0);

    return tour;
  }
};

#endif /* LINKERNIGHAN_H */
