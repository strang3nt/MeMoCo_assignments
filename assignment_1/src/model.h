#ifndef MODEL_H
#define MODEL_H

#include "graph.h"
#include "cpxmacro.h"
#include <ctime>
#include <sys/time.h>

class Model {

  public:
    struct TimeMeasures {
      double userTime;
      double cpuTime;
    };

    // creates variables and constraints
    
    TimeMeasures solveTsp(const Graph&);

  private:
    void initTsp(CEnv, Prob, const Graph&);
};

#endif
