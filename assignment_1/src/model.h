#ifndef MODEL_H
#define MODEL_H

#include "cpxmacro.h"
#include <vector>

struct TimeMeasures {
  double userTime;
  double cpuTime;
};

class Model {

  public:
    Model(Env _env, Prob _lp): env(_env), lp(_lp) {};

    // creates variables and constraints
    void initTsp(const int, const std::vector<std::vector<double>>&);
    TimeMeasures solveTsp(const int);

  protected:
    Env env;
    Prob lp;
};

#endif /* MODEL_H */
