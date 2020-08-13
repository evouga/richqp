# richqp

## Build Instructions

To build the code, use the standard CMake procedure:

````
mkdir build
cd build
cmake ..
make
````

## Run Instructions

Invoke the code from the command line as follows:

    richqp (budget) (betas file) (incomes file) (actual aids file) (output optimal aid file) (output equal-outcome aids file)
    
The betas file should be an array of coefficients for the objective function. The *i*th row and *j*th column is the coefficient of a^i inc^j. So for example
````
1.0 2.0
3.0 4.0
5.0 6.0
````
corresponds to the objective function 1.0 + 2.0\*inc + 3.0\*a + 4.0\*a\*inc + 5.0\*a\*a + 6.0\*a\*a\*inc. You can have as many columns as you want.

The incomes and actual aids files should be vectors of N whitespace-separated (spaces, tabs, or newlines) numbers. Do not include commas.

The program will write the N optimal and equal-outcome aid values into the two files you specify (one value per line).

## Example

To solve the Main Question for the University 1 in your notebook, you would run

    richqp 0.3 ../sample/samplebetas.txt ../sample/sampleincs.txt ../sample/sampleactaids.txt ../sample/optaids.txt ../sample/equalaids.txt

I created this per-student sample data set by simply copying each group income X_i times. The expected output is

````
Optimal aid allocation (with maximum total budget 1113.9) uses total budget 1113.9 and yields benefit 662.562
Actual aid allocation uses total budget 1722.27 and yields benefit 715.598
Optimal total budget needed to get benefit 715.598 is 1644.86
````