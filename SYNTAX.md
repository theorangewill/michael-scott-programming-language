# Syntax

### Variable types
```
Integer of 64 bits = 'PAM'
Integer of 32 bits = 'JIM'
Integer of 16 bits = 'ANDY'
Integer of 8 bits = 'ERIN'
Float of 65 bits = 'PHYLLIS'
Float of 32 bits = 'DWIGHT'
Boolean = 'STANLEY'
Void = 'CREED'
```

Not implemented yet:
```
Integer of 128 bits = 'DARRYL'
Float of 128 bits = 'MICHAEL'
Float of 16 bits = 'KEVIN'
Char = 'OSCAR'
String = 'KELLY'
Unsigned Integer of 64 bits = 'RYAN'
Unsigned Integer of 32 bits = 'ANGELA'
Unsigned Integer of 16 bits = 'TOBY'
Unsigned Integer of 8 bits = 'MEREDITH'
```
### Logic

```
or = 'snip'
and = 'snap'
not = 'no god no'
```

Not implemented yet:
```
True = 'WORLDS BEST BOSS'
False = 'ASSISTANT TO THE REGIONAL MANAGER'
```

### Comments

```
# This is a comment
```

### Variables and functions names

```
variables = 'paper' Number (ex: paper1, paper23123)
functions = 'chilli' Number (ex: chilli1, chilli213)

```

### Library include
'SABRE ACQUIRED ' '"' Library '"' ' and Dunder Mifflin'

Not implemented yet:
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
THATS WHAT SHE SAID chilli1('a', 'b', paper2)
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
JIM paper1, paper2
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
### Array Declaration
'GABE small talks to' Type Size 'times' ListVariableDeclaration

Not implemented yet:
```
GABE small talks to JIM 10 times paper1, paper2
```
### Loops

'FOR EACH ANGELAS CAT ' VariablesAssignment ':' Equation ':' VariablesAssignment ':' Body 'OH NO DWIGHT HAVE KILLED THEM'

```
FOR EACH ANGELAS CAT paper1=0:paper1 < paper2:paper1 = paper + 1:
//Body
OH NO DWIGHT HAVE KILLED THEM
```

'WHILE STANLEY IS SLEEPING and ' Equation ':' Body 'THE WORKING DAY IS OVER'

```
WHILE STANLEY IS SLEEPING and paper1 < paper2:
//Body
THE WORKING DAY IS OVER
```

'AND RYAN DOES:' Body 'WHEN KELLY IS TALKING and ' Equation ':'

```
AND RYAN DOES:
//Body
WHEN KELLY IS TALKING and paper1 < paper2:
```

### Control flow
'IF TOBY IS NOT HERE and' Equation ':' Body 'DAMN TOBY ARRIVED'

```
IF TOBY IS NOT HERE and paper1 < paper2:
//Body
DAMN TOBY ARRIVED
```

'HEI  TOBY IS HERE OH WAIT IT WASNT HIM SO IF ' Equation ':' Body

```
IF TOBY IS NOT HERE and paper1 < paper2:
//Body
HEI TOBY IS HERE OH WAIT IT WASNT HIM SO IF paper1 > paper2:
//Body
DAMN TOBY ARRIVED
```

'LOOK TOBY IS COMING:' Body

```
IF TOBY IS NOT HERE and paper1 < paper2:
//Body
HEI TOBY IS HERE OH WAIT IT WASNT HIM SO IF paper1 > paper2:
//Body
LOOK TOBY IS COMING:
//Body
DAMN TOBY ARRIVED
```

