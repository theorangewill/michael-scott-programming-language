# Michael Scott programming language

This is an [esoteric programming language](https://en.wikipedia.org/wiki/Esoteric_programming_language) based on the TV show The Office US using LLVM.

## Syntax

### Variable types
```
Integer = 'KELLY'
Long = 'GABE'
Void = 'CREED'
```

### Logic Operands

```
True = 'WORLDS BEST BOSS'
False = 'ASSISTANT TO THE REGIONAL MANAGER'
```

### Variables and functions names

```
variables = 'paper' Number (ex: paper1, paper23123)
functions = 'chilli' Number (ex: chilli1, chilli213)

```

### Library include
'SABRE ACQUIRED ' '"' Library '"' ' and Dunder Mifflin'

```
SABRE ACQUIRED "stdio" and Dunder Mifflin
```

### Global variable
'CORPORATE' VariableDeclaration

```
CORPORATE KELLY paper1, paper2
```
### Function
'DUNDER MIFFLIN THIS IS ' FunctionName '(' ListParameterOpt ')' FunctionBody 'FIRE!'

```
DUNDER MIFFLIN THIS IS chilli1(paper1, paper2, paper3)
//FunctionBody
FIRE!
```
#### Function return
'DUNDIE GOES TO ' VariableName

```
DUNDIE GOES TO paper1
```

#### Function call
'THATS WHAT SHE SAID ' FunctionName '(' ListVariableCall ')'

```
THATS WHAT SHE SAID chilli1('a', 'b', number)
```

#### Main declaration
'Scranton' '(' VariableOpt ')' FunctionBody 'FIRE!'

```
Scranton()
\\FunctionBody
FIRE!
```
## Statements

### Variable Declaration
Type ListVariableDeclaration

```
GABE paper1, paper2
```

### Variable Assignment
VariableName '=' FunctionCall 'SOLD'

```
paper1 = THATS WHAT SHE SAID chilli0() SOLD
```

VariableName '=' MathExpression 'SOLD'

```
paper1 = paper2 * 3333 + 444
```

### Loops

'FOR EACH ANGELAS CAT ' VariablesAssignment ':' Equation ':' VariablesAssignment ':' Body 'DWIGHT HAVE KILLED THEM'

```
FOR EACH ANGELAS CAT paper1=0:paper1 < paper2:paper1 = paper + 1:
//Body
DWIGHT HAVE KILLED THEM
```

'WHILE STANLEY IS SLEEPING and ' Equation ':' Body 'THE WORKING DAY IS OVER'

```
WHILE STANLEY IS SLEEPING and paper1 < paper2:
//Body
THE WORKING DAY IS OVER
```

'RYAN DOES:' Body 'WHILE KELLY IS TALKING and ' Equation ':'

```
RYAN DOES:
//Body
WHILE KELLY IS TALKING and paper1 < paper2:
```

### Control flow
'IF TOBY IS NOT HERE and' Equation ':' Body 'TOBY ARRIVED'

```
IF TOBY IS NOT HERE and paper1 < paper2:
//Body
TOBY ARRIVED
```

'TOBY IS HERE OH WAIT IT WASNT HIM SO IF ' Equation ':' Body

```
IF TOBY IS NOT HERE and paper1 < paper2:
//Body
TOBY IS HERE OH WAIT IT WASNT HIM SO IF paper1 > paper2:
//Body
TOBY ARRIVED
```

'TOBY IS COMING:' Body

```
IF TOBY IS NOT HERE and paper1 < paper2:
//Body
TOBY IS HERE OH WAIT IT WASNT HIM SO IF paper1 > paper2:
//Body
TOBY IS COMING:
//Body
TOBY ARRIVED
```

