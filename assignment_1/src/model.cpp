#include <cstdio>
#include <iostream>
#include <vector>
#include "cpxmacro.h"

using namespace std;

// error  status and message buffer
int status;
char errmsg[BUF_SIZE];

const int N = 4;

// matrix of nodes is complete
const int A = 16;

// (0, 0), (0, 1), (0, 2), (0, 3), 
// (1, 0), (1, 1), (1, 2), (1, 3),
// (2, 0), (2, 1), (2, 2), (2, 3),
// (3, 0), (3, 1), (3, 2), (3, 3)
// c(i, j) == c[i * N + j]
const double c[A] = {0, 1, 4, 2,
                  1, 0, 3, 6,
                  4, 3, 0, 5,
                  2, 6, 5, 0};
const int start_node = 0;

void setupLP(CEnv env, Prob lp) {
  // add y vars
  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {

			char ytype = 'B';
			double lb = 0.0;
			double ub = 1.0;
			// snprintf(name, NAME_SIZE, "x_%c_%c", nameI[i], nameJ[j]);
			// char* xname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &c[i * N + j], &lb, &ub, &ytype, NULL );
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
        // snprintf(name, NAME_SIZE, "x_%c", nameI[i]);
        // char* xname = (char*)(&name[0]);
        CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, NULL );
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
		for ( int j = 0; j < N; ++j ) {
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

int main (int argc, char const *argv[])
{
	try
	{
		// init
		DECL_ENV( env );
		DECL_PROB( env, lp );

		// setup LP
		setupLP(env, lp);

		// optimize
		CHECKED_CPX_CALL( CPXmipopt, env, lp );
		
		// print
		double objval;
		CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
		std::cout << "Objval: " << objval << std::endl;
		int n = CPXgetnumcols(env, lp);
		if (n != N*N) { throw std::runtime_error(std::string(__FILE__) + ":" + STRINGIZE(__LINE__) + ": " + "different number of variables"); }
	  	std::vector<double> varVals;
		varVals.resize(n);
  		CHECKED_CPX_CALL( CPXgetx, env, lp, &varVals[0], 0, n - 1 );
		
		/// status =      CPXgetx (env, lp, x          , 0, CPXgetnumcols(env, lp)-1);
  		for ( int i = 0 ; i < n ; ++i ) {
  	  	std::cout << "var in position " << i << " : " << varVals[i] << std::endl;
  	  		/// to read variables names the API function ``CPXgetcolname'' may be used (it is rather tricky, see the API manual if you like...)
  	  		/// status = CPXgetcolname (env, lp, cur_colname, cur_colnamestore, cur_storespace, &surplus, 0, cur_numcols-1);
  		}
		CHECKED_CPX_CALL( CPXsolwrite, env, lp, "ironrods.sol" );
		// free
		CPXfreeprob(env, &lp);
		CPXcloseCPLEX(&env);
	}
	catch(std::exception& e)
	{
		std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}
	return 0;
}
