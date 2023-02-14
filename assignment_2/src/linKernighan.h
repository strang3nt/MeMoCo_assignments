#ifndef LINKERNIGHAN_H
#define LINKERNIGHAN_H

#include <vector>
#include <tuple>
#include <deque>
#include <algorithm>
#include <iterator>
#include <set>
#include <stack>
#include <iostream>

class LinKernighan {

  public:
    // trick to have only edge (i, j) of symmetric tsp, instead of having (i, j) and (j, i)
    static const std::tuple<int, int>& makeTuple(int u, int v) {
      return u > v ? std::make_tuple(v, u) : std::make_tuple(u, v);
    }

    std::set<std::tuple<int, int>> makePairwise(std::vector<int> alternatingTrail, int i) {
      std::set<std::tuple<int, int>> alternatingTrailPairwise;
      for(int x = 0; x <= i - 1; x++) {
        alternatingTrailPairwise.insert(makeTuple(alternatingTrail.at(x), alternatingTrail.at(x + 1)));
      }
      return alternatingTrailPairwise;
    }

    const std::set<std::tuple<int, int>>& setUnion(const std::set<std::tuple<int, int>> &A, const std::set<std::tuple<int, int>>& B) {
      std::set<std::tuple<int, int>> result;
      std::set_union(A.begin(), A.end(), B.begin(), B.end(), std::inserter(result, result.begin())); 
      return result;
    }

    const std::set<std::tuple<int, int>>& setDifference(const std::set<std::tuple<int, int>> &A, const std::set<std::tuple<int, int>>& B) {
      std::set<std::tuple<int, int>> result;
      std::set_difference(A.begin(), A.end(), B.begin(), B.end(), std::inserter(result,result.begin()));
      return result;
    }

    const std::set<std::tuple<int, int>>& setSymmetricDifference(const std::set<std::tuple<int, int>> &A, const std::set<std::tuple<int, int>>& B) {
      std::set<std::tuple<int, int>> result;
      std::set_symmetric_difference(A.begin(), A.end(), B.begin(), B.end(), std::inserter(result,result.begin()));
      return result;
    }

    // the trail should be hamiltonian by construction
    bool isTour(const std::set<std::tuple<int,int>>& tour, int n) {
        
        // newTour should be |newTour| == |currentTour|
        std::vector<int> successor(n, -1);
        std::vector<bool> visited(n, false);
        
        int j = 0;
        
        for(int s = 0; s < n; ++s) {
          // find the successor of j I did not visit
          const auto& it = std::find_if(
            tour.begin(), 
            tour.end(),
            [&tour, s, j, &visited, n](const auto& edge){ 
              const auto [x, y] = edge;
              return (x == j || y == j) && (!visited.at(j == x ? y : x) || s == n - 1); });
          if(it != tour.end()) {
            visited.at(j) = true;
            const auto [x, y] = *it;
            successor.at(j) = x == j ? y : x;
            j = x == j ? y : x;
          }
        }
        
        return std::find(visited.begin(), visited.end(), false) == visited.end();
      }

    std::set<std::tuple<int, int>> localOptimalTour(
      std::set<std::tuple<int,int>> tour,
      std::vector<std::vector<double>> weight) {
      
      int p2 = 2;
      int p1 = 5;

      std::deque<std::tuple<int, int, double>> stack;
      std::vector<int> alternatingTrail(tour.size() * 2, -1);
      std::set<std::tuple<int, int>> F;
      double G = 0;

      do {
        G = 0;
        F.clear();
        for(size_t u = 0; u < weight.size(); ++u) { stack.push_front({u, 0, 0}); }
        
        while (!stack.empty()) {
          const auto [vi, i, gi] = stack.front();
          stack.pop_front();
          alternatingTrail.at(i) = vi;
          auto alternatingTrailPairwise = makePairwise(alternatingTrail, i);

          if (i % 2 == 0) {// if i is even

            for(size_t u = 0; u < weight.size(); ++u) {
              
              const auto& xi = makeTuple(vi, u);
              const auto& yi = makeTuple(u, alternatingTrail.at(0));
              const auto& setDifference_ = setDifference(tour, alternatingTrailPairwise);

              if(setDifference_.find(xi) != setDifference_.end()) {
                
                alternatingTrailPairwise.insert(xi);
                const auto& setUnion_ = setUnion(tour, alternatingTrailPairwise);
                bool notInTour = setUnion_.find(yi) == setUnion_.end();
                alternatingTrailPairwise.insert(yi);
                bool isTour_ = isTour(setSymmetricDifference(tour, alternatingTrailPairwise), tour.size());

                if(i <= p2 || (notInTour && isTour_)) {
                  stack.push_front({u, i + 1, gi + weight.at(vi).at(u)});
                }
                alternatingTrailPairwise.erase(xi);
                alternatingTrailPairwise.erase(yi);

              }
            }
          }

          else {
            
            double candidateG = gi - weight.at(vi).at(alternatingTrail.at(0));
            const auto& currTrail = setUnion(tour, alternatingTrailPairwise);
            const auto& endNode = makeTuple(vi, alternatingTrail.at(0));
            alternatingTrailPairwise.insert(endNode);
            bool isTour_ = isTour(setSymmetricDifference(tour, alternatingTrailPairwise), tour.size());
            if (candidateG > 0 && candidateG > G && isTour_) {
              F.clear();
              std::copy(alternatingTrailPairwise.begin(), alternatingTrailPairwise.end(), std::inserter(F, F.begin()));
              G = candidateG;
            }
            for(size_t u = 0; u < weight.size(); ++u) {
                if(gi > weight.at(vi).at(u) && currTrail.find(makeTuple(vi, u)) == currTrail.end())// and vi u is not in current trail
                  stack.push_front({u, i + 1, gi - weight.at(vi).at(u)});
            }
            alternatingTrailPairwise.erase(endNode);
          }
          
          int j = 0;
          std::tie(std::ignore, j, std::ignore) = stack.front();
          if(j <= i) {
            if(G > 0) {
              tour = setSymmetricDifference(tour, F);
              stack.clear();
            } else if(i > p1){
              while(std::get<1>(stack.front()) > p1) {
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
