#include "model.h"
#include <iostream>
#include <ctime>
#include <sys/time.h>

void Model::initTsp(Prob lp, const int N, const std::vector<std::vector<int>>& c) {

  // add y vars
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < N; ++j) {
			char ytype = 'B';
			double lb = 0.0;
			double ub = 1.0;
      double coef = i == j ? CPX_INFBOUND : c[i][j];
			CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &coef, &lb, &ub, &ytype, NULL);
    }
  }

  // add x vars x_0_1, x_0_2, x_0_3, x_1_1,... x_%_0 not considered
  for(int i = 0; i < N * (N - 1); ++i) {
      CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, NULL, NULL, NULL, NULL, NULL);
  }

  // forall k in N \ {0}, sum{ (i, k) in A } x_i_k - sum{ (k, j) in A, j != 0} x_k_j = 1
  for(int k = 0; k < N - 1; ++k) {
    std::vector<int> idx;
    std::vector<double> coef;
    for(int i = 0; i < N; ++i) { // x_i_k
      if(k + 1 != i) {
        idx.push_back(N * N + i * (N - 1) + k);
        coef.push_back(1.0);
      }
    }
    for(int j = 0; j < N - 1; ++j) { // x_k_j
      if(k != j) {
        idx.push_back(N * N + (k + 1) * (N - 1) + j);
        coef.push_back(-1.0);
      }
    }
    double rhs = 1.0;
    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL);
  }

  for(int i = 0; i < N; ++i) {
    std::vector<int> idx(N);
    for(int j = 0; j < N; ++j) { // y_i_j 
      idx[j] = i * N + j;
    }
    std::vector<double> coef(N, 1.0);
    double rhs = 1.0;
    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL);
  }

  for(int j = 0; j < N; ++j) {
    std::vector<int> idx(N);
    for(int i = 0; i < N; ++i) { // y_i_j 
      idx[i] = i * N + j;
    }
    std::vector<double> coef(N, 1.0);
    double rhs = 1.0;
    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, NULL, &matbeg, &idx[0], &coef[0], NULL, NULL);
  }

  for (int i = 0; i < N; ++i) {
		for (int j = 1; j < N; ++j) {
			std::vector<int> idx(2);
      idx[0] = i * N + j; // y_i_j
			idx[1] = N * N + i * (N - 1) + j - 1; // x_i_j
			std::vector<double> coef(2, 1.0);
			coef[0] = - N + 1.0;
			char sense = 'L';
			int matbeg = 0;
			CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, idx.size(), NULL, &sense, &matbeg, &idx[0], &coef[0], NULL, NULL);
		}
	}
  // CHECKED_CPX_CALL(CPXwriteprob, env, lp, "tsp.lp", NULL);
}

Results Model::solveTsp(const int N, const std::vector<std::vector<int>>& graph) {

	try {

    DECL_PROB(env, lp)

    initTsp(lp, N, graph);

    clock_t t1,t2;
    t1 = clock();
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);
    // solve
		CHECKED_CPX_CALL(CPXmipopt, env, lp);
    t2 = clock();
    gettimeofday(&tv2, NULL);
		double objval;
		CHECKED_CPX_CALL(CPXgetobjval, env, lp, &objval);
		int n = CPXgetnumcols(env, lp);
		if (n != N * N * 2 - N) { 
      throw std::runtime_error(std::string(__FILE__) + ":" + STRINGIZE(__LINE__) + ": " + "different number of variables"); 
    }
		// CHECKED_CPX_CALL(CPXsolwrite, env, lp, "TSP.sol");
    CPXfreeprob(env, &lp);

    return {
      objval,
      (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)),
      (double)(t2-t1) / CLOCKS_PER_SEC
    };
	}
	catch(std::exception& e) {
		std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}

  // if there is an exception, return user time and cpu time == -1
  return { -1, -1, -1 };
  
}
