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

    richqp (budget) (input betas file) (input incomes file) (output aids file)
    
The betas file should be an array of coefficients for the objective function. The *i*th row and *j*th column is the coefficient of a^i inc^j. So for example
````
1.0 2.0
3.0 4.0
5.0 6.0
````
corresponds to the objective function 1.0 + 2.0\*inc + 3.0\*a + 4.0\*a\*inc + 5.0\*a\*a + 6.0\*a\*a\*inc. You can have as many columns as you want.

The incomes file should be a vector of N whitespace-separated (spaces, tabs, or newlines) numbers. Do not include commas.

The program will write the N aid values into the file you provided (one value per line).

## Example

To solve the Main Question for the University 1 in your notebook, you would run

    richqp 0.3 ../sample/samplebetas.txt ../sample/sampleincs.txt ../sample/sampleaids.txt

I created this per-student sample data set by simply copying each group income X_i times.
