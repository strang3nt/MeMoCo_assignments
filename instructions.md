# Instructions for `assignment_1` and `assignment_2`

These are instructions to compile and execute both projects in
folders `assignment_1` and `assignment_2`.

## Compilation

From the root of the project's folder, from terminal, run `make`.

## Execution

From the root of the project's folder, from terminal, run:

 - `./main`, to parse and try to find a result for the 
 Travelling Salesperson Problem for all instances in `tsp_instances`; 
 - `./main` followed with the name of a file in `tsp_instances`, for example
  `./main 010_s_0.tsp` to execute only 1 instance.

# Instructions for `instance_generator`

`instance_generator` contains a script that generates instances for TSP, 
and in particular to the problem of drilling boards for electronic panels.
To compile and execute use the following commands, in the project's folder, 
first `make` and then `./main`, a prompt will appear.
The instance generator produces files according to the following input:

1. size of board, height and length
2. distance between holes coordinates
3. size of instance and number of instances
4. finally the name of the file, if more than 1 instance is required, 
the script will append a number to the name.

Inputs are supposed to be with a $10^{-5}$ precision, thus $10000$ is $10$ centimeters.
