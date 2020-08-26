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

    richqp (budget) (betas file) (incomes file) (actual aids file) (abilities file) (output optimal government aid file) (output optimal university aid file) (output equal-outcome aids file)
    
The betas file should be an array of coefficients for the objective function. In the first three rows, the *i*th row and *j*th column is the coefficient of a^i inc^j. So for example
````
1.0 2.0
3.0 4.0
5.0 6.0
````
corresponds to the objective function 1.0 + 2.0\*inc + 3.0\*a + 4.0\*a\*inc + 5.0\*a\*a + 6.0\*a\*a\*inc. You can have as many columns as you want.

The next three rows are in the same format, but linear in the student ability.

The incomes, actual aids, and abilities files should be vectors of N whitespace-separated (spaces, tabs, or newlines) numbers. Do not include commas.

The program will write the N optimal aid values into the three files you specify (one value per line).

## Example

To solve the Main Question for the University 1 in your notebook, you would run

    richqp 0.3 ../sample/samplebetas.txt ../sample/sampleincs.txt ../sample/sampleactaids.txt ../sample/sampleabilities.txt ../sample/optgovaids.txt ../sample/optuniaids.txt ../sample/equalaids.txt

I created this per-student sample data set by simply copying each group income X_i times. The expected output is

````
Optimal aid allocation (with government maximum budget 1113.9) yields benefit 455.496
Actual aid allocation uses total budget 1722.27 and yields benefit 508.532
Optimal aid allocation (with university maximum budget 1722.27) yields benefit 515.675
Optimal total budget needed to get benefit 508.532 is 1644.86
````