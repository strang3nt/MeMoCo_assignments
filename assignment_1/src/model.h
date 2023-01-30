#ifndef MODEL_H
#define MODEL_H

#include "cpxmacro.h"
#include "graph.h"

struct TimeMeasures {
  double userTime;
  double cpuTime;
};

class Model {

  public:
    Model(Env _env): env(_env) {};

    // creates variables and constraints
    
    TimeMeasures solveTsp(const Graph&);

  protected:
    void initTsp(Prob lp, const Graph&);
    Env env;
};

#endif
