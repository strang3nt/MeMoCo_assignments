#ifndef TRAIL_H
#define TRAIL_H

#include <set>
#include <tuple>
#include <vector>
#include <algorithm>
#include <memory>
#include <numeric>
#include <chrono>
#include <random>
#include <iostream>

#include "graph.h"

class Trail {

  std::set<std::tuple<int, int>> alternatingTrail;

  public:
    Trail(const std::set<std::tuple<int, int>>& _alternatingTrail): alternatingTrail(_alternatingTrail) {}

    Trail(const std::vector<int>& alternatingTrail, int i) {
    std::set<std::tuple<int, int>> alternatingTrailPairwise;
      for(int x = 0; x <= i - 1; x++) {
        alternatingTrailPairwise.insert(makeTuple(alternatingTrail.at(x), alternatingTrail.at(x + 1)));
      }
      this->alternatingTrail = alternatingTrailPairwise;
    }

    Trail(): alternatingTrail(std::set<std::tuple<int,int>>()) {}

    int cost(const Graph& g) const {
      int sum = 0;
      for(const auto &[x, y]: this->alternatingTrail) {
        sum += g.c[x][y];
        std::cout << "("<< x << ", " << y << ") ";

      }
      return sum;
    }

    static const std::unique_ptr<Trail> getRandomTour(const std::vector<std::vector<int>>& weight) {
      
      size_t n = weight.size();
      std::set<std::tuple<int,int>> tour;

      std::vector<int> numbers(n);
      std::iota(numbers.begin(), numbers.end(), 0);
      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::shuffle(numbers.begin(), numbers.end(), std::default_random_engine(seed));

      for(size_t j = 0; j < n - 1; ++j) {
        tour.insert(makeTuple(numbers.at(j), numbers.at(j + 1)));
      }
      tour.insert(makeTuple(numbers.at(0), *(numbers.end() - 1)));

      auto tourPtr = std::make_unique<Trail>(tour);
      return tourPtr;
    }

    static const std::tuple<int, int> makeTuple(int u, int v) {
      return u > v ? std::make_tuple(v, u) : std::make_tuple(u, v);
    }

    bool isTour(int n) const {

      std::vector<bool> visited(n, false);
      
      int j = 0;
      
      for(int s = 0; s < n; ++s) {
        // find the successor of j I did not visit
        const auto& it = std::find_if(
          this->alternatingTrail.begin(),   
          this->alternatingTrail.end(),
          [s, j, &visited, n](const auto& edge){ 
            const auto [x, y] = edge;
            int i = j == x ? y : x;
            return (x == j || y == j) && (!visited.at(i) || (s == n - 1 && i == 0)); });
        if(it != this->alternatingTrail.end()) {
          visited.at(j) = true;
          const auto [x, y] = *it;
          j = x == j ? y : x;
        }
      }
      
      return std::find(visited.begin(), visited.end(), false) == visited.end();
    }

    int size() {
      return this->alternatingTrail.size();
    } 

    bool contains(const std::tuple<int, int>& i) const {
      return alternatingTrail.find(i) != alternatingTrail.end();
    }

    void insert(const std::tuple<int, int>& i) {
      alternatingTrail.insert(i);
    }

    void erase(const std::tuple<int, int>& i) {
      alternatingTrail.erase(i);
    }

    std::unique_ptr<Trail> trailUnion(const Trail& otherTrail) const {
      std::set<std::tuple<int, int>> result;
      std::set_union(
        this->alternatingTrail.begin(), 
        this->alternatingTrail.end(), 
        otherTrail.alternatingTrail.begin(), 
        otherTrail.alternatingTrail.end(), 
        std::inserter(result, result.begin())); 
      auto trailPtr = std::make_unique<Trail>(result);
      return trailPtr;
    }

    std::unique_ptr<Trail> trailDifference(const Trail& otherTrail) const {
      std::set<std::tuple<int, int>> result;
      std::set_difference(
        this->alternatingTrail.begin(), 
        this->alternatingTrail.end(), 
        otherTrail.alternatingTrail.begin(), 
        otherTrail.alternatingTrail.end(), 
        std::inserter(result, result.begin())); 
      auto trailPtr = std::make_unique<Trail>(result);
      return trailPtr;
    }

    std::unique_ptr<Trail> trailSymmetricDifference(const Trail& otherTrail) const {
      std::set<std::tuple<int, int>> result;
      std::set_symmetric_difference(
        this->alternatingTrail.begin(), 
        this->alternatingTrail.end(), 
        otherTrail.alternatingTrail.begin(), 
        otherTrail.alternatingTrail.end(), 
        std::inserter(result, result.begin())); 
      auto trailPtr = std::make_unique<Trail>(result);
      return trailPtr;
    }

    static const std::unique_ptr<Trail> getRandomTourGreedy(const Graph& g) {
      std::set<std::tuple<int,int>> tour;
      const auto& weight = g.c;
      const size_t n = weight.size();
      srand (time(NULL));

      int i = rand() % n;
      int initial = i;
      std::vector<bool> visited(n, false);
      visited.at(i) = true;

      while (tour.size() < n - 1) {

        const auto orderedNeighbors = g.getNearestNeighbors(i);

        const auto firstNotVisited = std::find_if(
          orderedNeighbors.begin(),
          orderedNeighbors.end(),
          [&visited] (const auto& neighbor) { return !visited.at(neighbor); }
        );
        tour.insert({i, *firstNotVisited});

        visited.at(*firstNotVisited) = true;
        i = *firstNotVisited;

      }
      tour.insert({i, initial});

      auto tourPtr = std::make_unique<Trail>(tour);
      return tourPtr;
    }

};

#endif /* TOUR_H */