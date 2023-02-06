#ifndef MODEL_H
#define MODEL_H

#include "cpxmacro.h"
#include <vector>

struct Results {
  double result;
  double userTime;
  double cpuTime;
};

class Model {

  public:
    Model(Env _env): env(_env) {};

    Results solveTsp(const int, const std::vector<std::vector<double>>&);

  protected:
    Env env;
    void initTsp(Prob, const int, const std::vector<std::vector<double>>&);

};

#endif /* MODEL_H */
