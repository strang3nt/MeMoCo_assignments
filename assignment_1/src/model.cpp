#include <cstdio>
#include <iostream>
#include <vector>
#include "cpxmacro.h"
#include "model.h"
#include <algorithm>

void Model::initTsp(CEnv env, Prob lp, const Graph& tsp) {

  const int N = tsp.N;
  const std::vector<std::vector<double>> c = tsp.c;

  std::vector<char> nameN(N);
  for (int i = 0; i <= N; i++) {
    nameN.push_back((char) i);
  }

  const int NAME_SIZE = 512;
  char name[NAME_SIZE];

  // add y vars
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {

			char ytype = 'B';
			double lb = 0.0;
			double ub = 1.0;
			snprintf(name, NAME_SIZE, "y_%c_%c", nameN[i], nameN[j]);
			char* yname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &c[i][j], &lb, &ub, &ytype, &yname );
			/// status =      CPXnewcols (env, lp, ccnt, obj      , lb  , ub, xctype, colname);

    }
  }

  // add x vars
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {

        char xtype = 'C';
        double obj = 0.0;
        double lb = 0.0;
        double ub = CPX_INFBOUND;
			  snprintf(name, NAME_SIZE, "x_%c_%c", nameN[i], nameN[j]);
			  char* xname = (char*)(&name[0]);
        CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname);
    }
  }

  // forall k in N \ {0}, sum{ (i, k) in A } x_i_k - sum{ (k, j) in A, j != 0} x_k_j = 1
  for(int k = 1; k < N; k ++) {
    std::vector<int> idx(2 * N - 1);
    for(int i = 0; i < N; i++) { // x_i_k 
      idx[i] = N * N + i * N + k;
    }
    for(int j = 1; j < N; j++) { // x_k_j
      idx[j - 1 + N] = N * N + k * N + j;
    }
    std::vector<double> coef(2 * N, 1.0);
    for(int j = 0; j < N - 1; j++) {
      coef[j + N] = - 1.0;
    }

    char sense = 'E';
    double rhs = 1.0;
    int matbeg = 0;
    CHECKED_CPX_CALL(
      CPXaddrows, 
      env, 
      lp, 
      0, 
      1, 
      idx.size(), 
      &rhs,
      &sense, 
      &matbeg, 
      &idx[0], 
      &coef[0], 
      NULL, 
      NULL);
  }

  for(int i = 0; i < N; i++) {
    std::vector<int> idx(N);
    for(int j = 0; j < N; j++) { // y_i_j 
      idx[j] = i * N + j;
    }
    std::vector<double> coef(N, 1.0);
    char sense = 'E';
    double rhs = 1.0;
    int matbeg = 0;
        CHECKED_CPX_CALL(
      CPXaddrows, 
      env, 
      lp, 
      0, 
      1, 
      idx.size(), 
      &rhs,
      &sense, 
      &matbeg, 
      &idx[0], 
      &coef[0], 
      NULL, 
      NULL);
  }

  for(int j = 0; j < N; j++) {
    std::vector<int> idx(N);
    for(int i = 0; i < N; i++) { // y_i_j 
      idx[i] = i * N + j;
    }
    std::vector<double> coef(N, 1.0);
    char sense = 'E';
    double rhs = 1.0;
    int matbeg = 0;
        CHECKED_CPX_CALL(
      CPXaddrows, 
      env, 
      lp, 
      0, 
      1, 
      idx.size(), 
      &rhs,
      &sense, 
      &matbeg, 
      &idx[0], 
      &coef[0], 
      NULL, 
      NULL);
  }

  for ( int i = 0; i < N; ++i ) {
		for ( int j = 1; j < N; ++j ) {
			std::vector<int> idx(2);
      idx[0] = N * N + i * N + j; // x_i_j
			idx[1] = i * N + j; // y_i_j
			
			std::vector<double> coef(2);
			coef[0] = 1.0;
			coef[1] = -(abs(N) - 1.0);
			char sense = 'L';
			double rhs = 0.0;
			int matbeg = 0;
			CHECKED_CPX_CALL(
        CPXaddrows, 
        env, 
        lp, 
        0, 
        1, 
        idx.size(), 
        &rhs,
        &sense,
        &matbeg,
        &idx[0], 
        &coef[0], 
        NULL, 
        NULL);
		}
	}
}


Model::TimeMeasures Model::solveTsp(const Graph& tsp) {

	try {
		// init
		DECL_ENV( env );
		DECL_PROB( env, lp );

		// setup LP
		initTsp(env, lp, tsp);

    clock_t t1,t2;
    t1 = clock();
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);
    
    // solve
		CHECKED_CPX_CALL( CPXmipopt, env, lp );

    t2 = clock();
    gettimeofday(&tv2, NULL);

		
		// print
		double objval;
		CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
		std::cout << "Objval: " << objval << std::endl;
		int n = CPXgetnumcols(env, lp);
		if (n != tsp.N*tsp.N*2) { throw std::runtime_error(std::string(__FILE__) + ":" + STRINGIZE(__LINE__) + ": " + "different number of variables"); }
	  std::vector<double> varVals;
		varVals.resize(n);
    CHECKED_CPX_CALL( CPXgetx, env, lp, &varVals[0], 0, n - 1 );
		
		/// status =      CPXgetx (env, lp, x          , 0, CPXgetnumcols(env, lp)-1);
    for ( int i = 0 ; i < n ; ++i ) {
      std::cout << "var in position " << i << " : " << varVals[i] << std::endl;
    }
		CHECKED_CPX_CALL( CPXsolwrite, env, lp, "TSP.sol" );
		// free
		CPXfreeprob(env, &lp);
		CPXcloseCPLEX(&env);

    return {
      (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)),
      (double)(t2-t1)
    };
	}
	catch(std::exception& e) {
		std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}

  // if there is a runtime error, return user time and cpu time == 0
  return { 0.0, 0.0 };
  
}
