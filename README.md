# Michael Scott programming language

This is an [esoteric programming language](https://en.wikipedia.org/wiki/Esoteric_programming_language) based on the TV show The Office US using LLVM.

## Getting Started

1- Compile the compiler:
```
git clone https://github.com/theorangewill/michael-scott-programming-language.git
cd michael-scott-programming-language/code
make
cd ..
```
2- Write your program with .ms extension

3- Compile your code
```
./code/MScompiler yourcode.ms yourobject.o
```
It will be generated a object file called yourobject.o

4- Create the executable file with clang++
```
clang++ yourobject.o -o yourprogram
```

5- Run your program
```
./yourprogram
```

6- You can link different object files with clang++ and use functions from other programs
```
./code/MScompiler yourcode1.ms yourobject1.o
./code/MScompiler yourcode2.ms yourobject2.o
clang++ yourobject1.o yourobject2.o -o yourprogram
./yourprogram
```

## Repository Structure
```
code/   <- the whole code
    ast.h       <- abstract syntax tree base code
    lexic.h     <- token class for lexing
    llvm.cpp    <- LLVM base code
    llvm.h      <- LLVM base code
    main.cpp
    main.h
    Makefile
    michaelscottgrammar.cpp    <- the code generator using LLVM
    michaelscottgrammar.h      <- the grammar classes
    parser.cpp    <- lexical and syntactic analysis
    parser.h      <- parser class
programs/       <- examples
SYNTAX.md       <- the language's syntax
Grammar         <- the language's grammar
TODO            <- what is missing?
```



### Example programs
In programs folder there are some examples.

```
birthday.ms         <- calculates how many years you have
bmi.ms              <- calculates your BMI
compare-numbers.ms  <- check if two given numbers
factorial.ms        <- calculates the factorial of a number
fibonacci-non-recursive.ms <- calculates fibonacci of a number
fibonacci-recursive.ms <- calculates fibonacci of a number in a recursive implementation
get-out.ms          <- a challenge
helloworld.ms
multiple.ms         <- calculates the multiples of a number in a range
print-numbers.ms    <- prints al the number from 0 to the given number
```
 


## How to make my own language?

You can start by learning about lexing and parsing [here](https://www.freecodecamp.org/news/the-programming-language-pipeline-91d3f449c919/).

To make a executable, basically you can make a converter from your language syntax to another language that has a compiler, or you can use LLVM to make your own compiler. There is a tutorial to create your own language [here](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html).

### Important commands of LLVM

Sometimes it is usual to see how clang converts a C++ code to LLVM IR:
```
clang++ -O0 -emit-llvm -S CPLUSPLUSCODE.cpp -o /dev/stdout
```

Another command is llc, that converts a LLVM IR code to object file:
```
llc LLVMIRCODE.ll
```
