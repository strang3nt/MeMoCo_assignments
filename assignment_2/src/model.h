#ifndef MODEL_H
#define MODEL_H

#include "cpxmacro.h"
#include <vector>
#include <set>
#include <tuple>

struct Results {
  double result;
  double userTime;
  double cpuTime;
};

class Model {

  public:
    Model(Env _env): env(_env) {};

    const Results solveTsp(const int, const std::vector<std::vector<double>>&) const;
    const std::set<std::tuple<int, int>> getFeasibleSolution(const int, const std::vector<std::vector<double>>&, const int) const;


  protected:
    Env env;
    void initTsp(Prob, const int, const std::vector<std::vector<double>>&) const;

};

#endif /* MODEL_H */
