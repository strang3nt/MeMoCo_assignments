#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <random>


//  1. build a list of coordinates
//   1. ask size of board L x H in millimiters
//   1. ask distance between coordinates
//   2. ask how many random instances
//  2. select n < list.size points
//  3. print those points into a file


int main(int argc, char const *argv[]) {


  int board_height = 0;
  int board_length = 0;
  int coords_dist = 0;

  std::cout << "Input size of board, first height, then length: ";
  std::cin >> board_height;
  std::cin >> board_length;
  std::cout << "Input distance between holes coordinates: ";
  std::cin >> coords_dist;

  int points_height = board_height / coords_dist;
  int points_length = board_length / coords_dist;

  std::vector<std::tuple<int, int>> points;
  for(int i = 0; i < points_height; ++i) {
    for(int j = 0; j < points_length; ++j) {
      points.push_back(std::make_tuple(i * coords_dist, j * coords_dist));
    }
  }
 
  for(auto [i, j] : points) {
    std::cout << i << ", " << j << "\n";
  }
  std::random_device rd;
  std::mt19937 g(rd());


  int size_of_instance = 0;
  int number_of_instances = 0;

  std::cout << "Input desired size of instances, and number of random instances: ";
  std::cin >> size_of_instance;
  std::cin >> number_of_instances;
  std::string filename;
  std::cout << "Input root name of files: ";
  std::cin >> filename;


  for(int i = 0; i < number_of_instances; ++i) {

    std::shuffle(points.begin(), points.end(), g);

    std::ofstream myfile;
    
    myfile.open (filename + "_" + std::to_string(i) + ".tsp");
    myfile << size_of_instance + "\n";

    for(int j = 1; j <= size_of_instance; ++j) {
          std::cout << std::get<0>(points[j - 1]) + " "
             << std::get<1>(points[j - 1]) + "\n";
      myfile << j << " "
             << std::get<0>(points[j - 1]) << " "
             << std::get<1>(points[j - 1]) << "\n";
    }

    myfile.close();
  }
  
}
