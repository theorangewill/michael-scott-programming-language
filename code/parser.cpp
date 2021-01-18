#include "parser.h"

Parser::Parser(){
    MathopPrecedence["+"] = 20;
    MathopPrecedence["-"] = 20;
    MathopPrecedence["*"] = 40; 
    MathopPrecedence["/"] = 40; 
    MathopPrecedence["%"] = 60; 
    LogicopPrecedence["OR"] = 20;
    LogicopPrecedence["AND"] = 40;
    LogicopPrecedence["NOT"] = 60;
    TypesDefinition["GABE"] = "double";
    TypesDefinition["KELLY"] = "short int";
};

void Parser::error()
{
  printf("**Error [line %d]: unknown\n", lineNumber);
  exit(1);
}

void Parser::error(const char* error_warning)
{
  printf("**Error [line %d]: %s\n", lineNumber, error_warning);
  exit(1);
}

void Parser::error(const char* error_warning, std::string error_text)
{
  printf("**Error [line %d]: %s\n\t%s\n", lineNumber, error_warning, error_text.c_str());
  exit(1);
}

// It parses a support text
int Parser::parserSupport(const char* support_text){
  std::string buff{""};
  std::vector<std::string> text_aux;
  
  // Splits the support text with space delimiter
  for(auto n:std::string(support_text)){
    if(n != ' ') 
      buff+=n; 
    else if(n == ' ' && buff != ""){
      text_aux.push_back(buff); 
      buff = ""; 
    }
  }
  if(buff != "") 
    text_aux.push_back(buff);

  int index = 0;
  std::string identifier_aux;
  char* buffer_aux;
  bool correct;
  int parser_aux = 0;
  buffer_aux = buffer;
  IdentifierStr = "";
  do{
    correct = false;
    identifier_aux = *buffer_aux;
    while (isalnum((*buffer_aux++))){
      identifier_aux += *buffer_aux;
    }
    if(text_aux[index] == identifier_aux.substr(0, identifier_aux.size()-1)){
      IdentifierStr += identifier_aux;
      correct = true;
      parser_aux++;
    }
    index++;
  }while(correct && index < text_aux.size());

  if(correct){
    IdentifierStr = IdentifierStr.substr(0, IdentifierStr.size()-1);
    buffer = buffer_aux;
    parser_aux = 1;
  }
  else if(parser_aux > 0) parser_aux = -1;
  else parser_aux = 0;
  return parser_aux;
}

Token Parser::nextToken()
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
  while (isspace(*buffer)){
    if(*buffer == '\n') lineNumber++;
    *buffer++;
  }

  if (*buffer == '\0') return Token(Token::EOF_, std::string("the end"));
  
  char c = *buffer++;
  char c2;
  if(c == '(' || c == ')' || c == ':' || c == '!' || c == ',')
    return Token(Token::tok_symbol, std::string(1, c));
  else if(c == '+' || c == '/' || c == '*' || c == '%')
    return Token(Token::tok_mathop, std::string(1, c));
  else if(c == '<' || c == '>'){
    c2 = *buffer++;
    if(c2 == '=')
      return Token(Token::tok_compop, std::string()+c+c2);
    *buffer--;
    return Token(Token::tok_compop, std::string()+c);
  }
  else if(c == '='){
    c2 = *buffer++;
    if(c2 == '=' || c2 == '!')
      return Token(Token::tok_compop, std::string()+c+c2);
    *buffer--;
    return Token(Token::tok_attr, std::string(1, c));
  }
  *buffer--;

  int support_aux;
  if (isalpha(*buffer)) {
    // If it is a library include
    if((support_aux = parserSupport("SABRE ACQUIRED"))){
      if(support_aux < 0)
        error("Wrong support for library include. It should be 'SABRE ACQUIRED'", IdentifierStr);
      *buffer--;
      return Token(Token::tok_include_support, IdentifierStr);
    }
    // If it is end of library include
    if((support_aux = parserSupport("and Dunder Mifflin"))){
      if(support_aux < 0)
        error("Wrong support for library include. It should be 'and Dunder Mifflin'", IdentifierStr);
      *buffer--;
      return Token(Token::tok_include_support, IdentifierStr);
    }
    // If it is a function return
    if((support_aux = parserSupport("DUNDIE GOES TO"))){
      if(support_aux < 0)
        error("Wrong support for function return. It should be 'DUNDIE GOES TO'", IdentifierStr);
      *buffer--;
      return Token(Token::tok_function_support, IdentifierStr);
    }
    // If it is a function definition
    if((support_aux = parserSupport("DUNDER MIFFLIN THIS IS"))){
      if(support_aux < 0)
        error("Wrong support for function definition. It should be 'DUNDER MIFFLIN THIS IS'", IdentifierStr);
      *buffer--;
      return Token(Token::tok_function_support, IdentifierStr);
    }
    // If it is a If statement
    if((support_aux = parserSupport("IF TOBY IS NOT HERE and"))){
      if(support_aux < 0)
        error("Wrong If statement support. It should be 'IF TOBY IS NOT HERE and'");
      *buffer--;
      return Token(Token::tok_statement_support, IdentifierStr);
    }
    // If it is Else If statement
    if((support_aux = parserSupport("TOBY IS HERE OH WAIT IT WASNT HIM SO IF"))){
      if(support_aux > 0){
        *buffer--;
        return Token(Token::tok_statement_support, IdentifierStr);
      }
    }
    // If it is Else statement
    if((support_aux = parserSupport("TOBY IS COMING"))){
      if(support_aux > 0){
        *buffer--;
        return Token(Token::tok_statement_support, IdentifierStr);
      }
    }
    // If it is end of If statement
    if((support_aux = parserSupport("TOBY ARRIVED"))){
      if(support_aux < 0)
        error("Wrong If statement end support. It should be 'TOBY ARRIVED'. If you meant an Else statement support, it should be 'TOBY IS COMING'. If you ment a Else If statement support, it should be 'TOBY IS HERE OH WAIT IT WASNT HIM SO IF'");
      *buffer--;
      return Token(Token::tok_statement_support, IdentifierStr);
    }
    // If it is For statement
    if((support_aux = parserSupport("FOR EACH ANGELAS CAT"))){
      if(support_aux < 0)
        error("Wrong For statement support. It should be 'FOR EACH ANGELAS CAT'");
      *buffer--;
      return Token(Token::tok_statement_support, IdentifierStr);
    }
    // If it is end of For statement
    if((support_aux = parserSupport("DWIGHT HAVE KILLED THEM"))){
      if(support_aux < 0)
        error("Wrong For statement end support. It should be 'DWIGHT HAVE KILLED THEM'");
      *buffer--;
      return Token(Token::tok_statement_support, IdentifierStr);
    }
    // If it is While statement
    if((support_aux = parserSupport("WHILE STANLEY IS SLEEPING and"))){
      if(support_aux > 0){
        *buffer--;
        return Token(Token::tok_statement_support, IdentifierStr);
      }
    }
    // If it is end of While statement
    if((support_aux = parserSupport("THE WORKING DAY IS OVER"))){
      if(support_aux < 0)
        error("Wrong While statement end support. It should be 'THE WORKING DAY IS OVER'");
      *buffer--;
      return Token(Token::tok_statement_support, IdentifierStr);
    }
    // If it is Do While statement
    if((support_aux = parserSupport("RYAN DOES"))){
      if(support_aux < 0)
        error("Wrong Do While statement support. It should be 'RYAN DOES'");
      *buffer--;
      return Token(Token::tok_statement_support, IdentifierStr);
    }
    // If it is end of Do While statement
    if((support_aux = parserSupport("WHILE KELLY IS TALKING and"))){
      if(support_aux < 0)
        error("Wrong Do While statement end support. It should be 'WHILE KELLY IS TALKING and'. If you meant an While statement support, it should be 'WHILE STANLEY IS SLEEPING and'");
      *buffer--;
      return Token(Token::tok_statement_support, IdentifierStr);
    }
    // If it is a function call
    if((support_aux = parserSupport("THATS WHAT SHE SAID"))){
      if(support_aux < 0)
        error("Wrong support for function call. It should be 'THATS WHAT SHE SAID'");
      return Token(Token::tok_function_support, IdentifierStr);
    }
    // If it is False
    if((support_aux = parserSupport("ASSISTANT TO THE REGIONAL MANAGER"))){
      if(support_aux < 0)
        error("Wrong False definition. It should be 'ASSISTANT TO THE REGIONAL MANAGER'");
      *buffer--;
      return Token(Token::tok_false, IdentifierStr);
    }
    // If it is True
    if((support_aux = parserSupport("WORLDS BEST BOSS"))){
      if(support_aux < 0)
        error("Wrong True definition. It should be 'WORLDS BEST BOSS'");
      *buffer--;
      return Token(Token::tok_true, IdentifierStr);
    }
    // If it is main
    if((support_aux = parserSupport("Scranton"))){
      *buffer--;
      return Token(Token::tok_main, IdentifierStr);
    }
    // If it is end of function
    if((support_aux = parserSupport("FIRE"))){
      *buffer--;
      return Token(Token::tok_function_support, IdentifierStr);
    }
    // If it is end of an assignment
    if((support_aux = parserSupport("SOLD"))){
      *buffer--;
      return Token(Token::tok_support, IdentifierStr);
    }
    // If it is AND
    if((support_aux = parserSupport("AND"))){
      *buffer--;
      return Token(Token::tok_logicop, IdentifierStr);
    }
    // If it is OR
    if((support_aux = parserSupport("OR"))){
      *buffer--;
      return Token(Token::tok_logicop, IdentifierStr);
    }
    // If it is NOT
    if((support_aux = parserSupport("NOT"))){
      *buffer--;
      return Token(Token::tok_notop, IdentifierStr);
    }
    // If it is a global variable
    if((support_aux = parserSupport("CORPORATE"))){
      return Token(Token::tok_type, IdentifierStr);
    }
    // If it is a short variable type
    if((support_aux = parserSupport("KELLY"))){
      return Token(Token::tok_type, IdentifierStr);
    }
    // If it is a long variable type
    if((support_aux = parserSupport("GABE"))){
      return Token(Token::tok_type, IdentifierStr);
    }
    // If it is a void variable type
    if((support_aux = parserSupport("CREED"))){
      return Token(Token::tok_type, IdentifierStr);
    }
    // Get the identifier
    IdentifierStr = *buffer;
    while (isalnum((*buffer++))){
      IdentifierStr += *buffer;
    }
    IdentifierStr = IdentifierStr.substr(0, IdentifierStr.size()-1);
    *buffer--;
    // If it is a function name
    if(!IdentifierStr.compare(0, 6, "chilli")){
      return Token(Token::tok_function, IdentifierStr);
    }
    // If it is a variable name
    if(!IdentifierStr.compare(0, 5, "paper")){
      return Token(Token::tok_variable, IdentifierStr);
    }
    error("Wrong support text, type name, variable name or function name.");
  }
/*
  if (isdigit(*buffer) || *buffer == '.') { // Number: [0-9.]+
    std::string NumStr;
    do {
      NumStr += *buffer;
      *buffer++;
    } while (isdigit(*buffer) || *buffer == '.');

    NumVal = strtod(NumStr.c_str(), nullptr);
    thisToken.type = -1000;
    thisToken.lexeme = NumStr;
    return thisToken;
  }
*/
  char* begin_digit = buffer;
  // If it is a digit (it can begins with . - [0-9])
  if (*buffer == '.' || *buffer == '-' || isdigit(*buffer))
  {
    if(*buffer == '-')
      buffer++;

    if(*buffer != '0'){
      ++buffer;
      while(isdigit(*buffer))
        ++buffer;
    }
    else
        ++buffer;


    if(*buffer == '.'){
      buffer++;
      if(isdigit(*buffer))
        buffer++;
      else
	      error("Wrong number.", std::string(begin_digit, buffer));
      while (isdigit(*buffer))
        buffer++;
    }

    if(*buffer == 'e' || *buffer == 'E'){
      buffer++;
      if(*buffer == '+' || *buffer == '-')
      buffer++;
      if(isdigit(*buffer))
        buffer++;
      else
	      error("Wrong number.", std::string(begin_digit, buffer));
      while (isdigit(*buffer))
        buffer++;
    }
    return Token(Token::tok_number, std::string(begin_digit, buffer));
  }

  char* begin_string = buffer;
  // If it is a string
  if(*buffer == '"'){
    buffer++;
    while(*buffer != '"'){
      if (*buffer == 9 || *buffer == 10){
        error();
      }
      else if(*buffer == 92){
        buffer++;
        if(*buffer == 34 || *buffer == 10 || *buffer == 9 || *buffer == 92){
         buffer++;
        }
        else
         error();
      }
      else
        buffer++;
    }
    buffer++;
    return Token(Token::tok_string, std::string(begin_string, buffer));
  }

  // If it is a comment
  if (*buffer == '#') {
    do
      *buffer++;
    while(*buffer != EOF && *buffer != '\n' && *buffer != '\r');
    if(*buffer != EOF) return nextToken();
  }

	error();
  return Token(Token::ERROR, "");
#pragma clang diagnostic pop
}

MichaelScott* Parser::parse(const char* input)
{
  buffer = const_cast<char*>(input);
  lineNumber = 1;
  MichaelScott* mc = michaelscott();
  if (lookahead.type != Token::EOF_)
    error();
  return mc;
}

MichaelScott* Parser::michaelscott()
{
	MichaelScott* mc = new MichaelScott();
  mc->c = components();
	if(lookahead.type == Token::EOF_)
		advance();
	else
		error();
  printf("<<<<<MichaelScott");
  return mc;
}

Components* Parser::components()
{
	Components* c = new Components();
  advance();
	while(true){
		c->add(component());
    printf("%s\n", lookahead.lexeme.c_str());
		if(lookahead.type == Token::EOF_){
      printf("<<<<<Components");
			return c;
    }
	}
}

Component* Parser::component()
{
  Component* c = new Component();
  
  if(lookahead.type == Token::tok_include_support)
    c->i = include();
  else if(lookahead.type == Token::tok_support)
    c->gvd = globalvariabledeclaration();
  else if(lookahead.type ==  Token::tok_function_support
          || lookahead.type ==  Token::tok_main)
    c->f = function();
  else
    error("You should declare a global variable, include a library or describe a function");
  printf("<<<<<Component");
  return c;
}

Include* Parser::include()
{
	Include* i = new Include();
  match("SABRE ACQUIRED");
  advance();
  match(Token::tok_string);
  i->library = lookahead.lexeme;
  advance();
  match("and Dunder Mifflin");
  advance();
  printf("<<<<<Include");
  return i;
}

GlobalVariableDeclaration* Parser::globalvariabledeclaration()
{
	GlobalVariableDeclaration* gvd = new GlobalVariableDeclaration();
  match("CORPORATE");
  advance();
  gvd->vd = variabledeclaration();
  printf("<<<<<GlobalVariableDeclaration");
  return gvd;
}

Function* Parser::function()
{
	Function* f = new Function();
  if(lookahead.lexeme == "DUNDER MIFFLIN THIS IS"){
    advance();
    match(Token::tok_type);
    if (TypesDefinition.find(lookahead.lexeme) != TypesDefinition.end()) {
      f->return_type = TypesDefinition[lookahead.lexeme];
    } else {
      error("Type not defined", lookahead.lexeme);
    }
    advance();
    match(Token::tok_function);
  }
  else if(lookahead.lexeme == "Scranton"){
    match(Token::tok_main);
  }
  else
    error("Wrong function definition");
  f->name = lookahead.lexeme;
  advance();
  match("(");
  f->p = parameters();
  match(")");
  f->body = functioncomponents();
  match("FIRE");
  advance();
  match("!");
  advance();
  printf("<<<<<Function");
  return f;
}

Parameters* Parser::parameters()
{
	Parameters* p = new Parameters();
	while(true) {
    advance();
		if(lookahead.lexeme == ")") return p; 
		p->add(parameter());
    advance();
		if(lookahead.lexeme != ","){    
      printf("<<<<<Parameters");
      return p;
    }
	}
}

Parameter* Parser::parameter()
{
  Parameter* p = new Parameter();
  match(Token::tok_type);
  if (TypesDefinition.find(lookahead.lexeme) != TypesDefinition.end()) {
    p->type = TypesDefinition[lookahead.lexeme];
  } else {
    error("Type not defined", lookahead.lexeme);
  }
  advance();
  match(Token::tok_variable);
  p->name = lookahead.lexeme;
  printf("<<<<<Parameter");
  return p;
}

FunctionComponents* Parser::functioncomponents()
{
	FunctionComponents* c = new FunctionComponents();
  advance();
	while(true) {
		c->add(functioncomponent());
		if (lookahead.lexeme == "FIRE"){    
      printf("<<<<<FunctionComponents");
      return c;
    }
	}
}

FunctionComponent* Parser::functioncomponent()
{
  FunctionComponent* c = new FunctionComponent();
	switch(lookahead.type){
    case Token::tok_type:
      c->vd = variabledeclaration();
      break;
    case Token::tok_function_support:
      if(lookahead.lexeme == "THATS WHAT SHE SAID")
        c->fc = functioncall();
      else if(lookahead.lexeme == "DUNDIE GOES TO")
        c->fr = functionreturn();
      break;
    case Token::tok_statement_support:
      c->s = statement();
      break;
    case Token::tok_variable:
      c->a = assignment();
      break;
    default:
      error("You should declare a variable, create an assignment, call a function or put a return");
  }
  printf("<<<<<FunctionComponent");
  return c;
}

FunctionCall* Parser::functioncall()
{
  FunctionCall* fc = new FunctionCall();
  match("THATS WHAT SHE SAID");
  advance();
  match(Token::tok_function);
  fc->name = lookahead.lexeme;
  advance();
  match("(");
  fc->args = arguments();
  match(")");
  advance();
  printf("<<<<<FunctionCall");
  return fc;
}

Arguments* Parser::arguments()
{
	Arguments* a = new Arguments();
	while(true) {
    advance();
		if(lookahead.lexeme == ")") return a; 
		a->add(argument());
    advance();
		if (lookahead.lexeme != ","){
      printf("<<<<<Arguments");
      return a;
    }
	}
}

Argument* Parser::argument()
{
  Argument* a = new Argument();
  match(Token::tok_variable);
  a->name = lookahead.lexeme;
  printf("<<<<<Argument");
  return a;
}

FunctionReturn* Parser::functionreturn()
{
  FunctionReturn* fr = new FunctionReturn();
  match("DUNDIE GOES TO");
  advance();
  match(Token::tok_variable);
  fr->v = lookahead.lexeme;
  advance();
  printf("<<<<<FunctionReturn");
  return fr;
}

VariableDeclaration* Parser::variabledeclaration()
{
  VariableDeclaration* vd = new VariableDeclaration();
  match(Token::tok_type);
  if (TypesDefinition.find(lookahead.lexeme) != TypesDefinition.end()) {
    vd->type = TypesDefinition[lookahead.lexeme];
  } else {
    error("Type not defined", lookahead.lexeme);
  }
  vd->v = variables(lookahead.lexeme);
  printf("<<<<<VariableDeclaration");
  return vd;
}

Variables* Parser::variables(std::string variable_type)
{
	Variables* v = new Variables();
	while(true) {
    advance();
		v->add(variable(variable_type));
    advance();
		if (lookahead.lexeme != ",") {
      printf("<<<<<Variables");
      return v;
    }
	}
}

Variable* Parser::variable(std::string variable_type)
{
  Variable* v = new Variable();
  if (TypesDefinition.find(variable_type) != TypesDefinition.end()) {
    v->type = TypesDefinition[variable_type];
  } else {
    error("Type not defined");
  }
  match(Token::tok_variable);
  v->name = lookahead.lexeme;
  printf("<<<<<Variable");
  return v;
}

StatementComponents* Parser::ifstatementcomponents()
{
	StatementComponents* c = new StatementComponents();
  advance();
  if (lookahead.lexeme == "TOBY IS HERE OH WAIT IT WASNT HIM SO IF"
      || lookahead.lexeme == "TOBY IS COMING"
      || lookahead.lexeme == "TOBY ARRIVED")
      return c;
	while(true) {
		c->add(statementcomponent());
		if (lookahead.lexeme == "TOBY IS HERE OH WAIT IT WASNT HIM SO IF"
      || lookahead.lexeme == "TOBY IS COMING"
      || lookahead.lexeme == "TOBY ARRIVED"){    
      printf("<<<<<StatementComponents");
      return c;
    }
	}
}

StatementComponents* Parser::forstatementcomponents()
{
	StatementComponents* c = new StatementComponents();
  advance();
  if (lookahead.lexeme == "DWIGHT HAVE KILLED THEM")
    return c;
	while(true) {
		c->add(statementcomponent());
		if (lookahead.lexeme == "DWIGHT HAVE KILLED THEM"){    
      printf("<<<<<StatementComponents");
      return c;
    }
	}
}

StatementComponents* Parser::whilestatementcomponents()
{
	StatementComponents* c = new StatementComponents();
  advance();
  if (lookahead.lexeme == "THE WORKING DAY IS OVER")
    return c;
	while(true) {
		c->add(statementcomponent());
		if (lookahead.lexeme == "THE WORKING DAY IS OVER"){    
      printf("<<<<<StatementComponents");
      return c;
    }
	}
}

StatementComponents* Parser::dowhilestatementcomponents()
{
	StatementComponents* c = new StatementComponents();
  advance();
  if (lookahead.lexeme == "WHILE KELLY IS TALKING and")
    return c;
	while(true) {
		c->add(statementcomponent());
		if (lookahead.lexeme == "WHILE KELLY IS TALKING and"){    
      printf("<<<<<StatementComponents");
      return c;
    }
	}
}

StatementComponent* Parser::statementcomponent()
{
  StatementComponent* c = new StatementComponent();
	switch(lookahead.type){
    case Token::tok_type:
      c->vd = variabledeclaration();
      break;
    case Token::tok_function_support:
      if(lookahead.lexeme == "THATS WHAT SHE SAID")
        c->fc = functioncall();
      else if(lookahead.lexeme == "DUNDIE GOES TO")
        c->fr = functionreturn();
      else
        error("You should create a body for this statement. If you tried to call a function or make a return, the syntax is wrong");
      break;
    case Token::tok_statement_support:
      c->s = statement();
      break;
    case Token::tok_variable:
      c->a = assignment();
      break;
    default:
      error("You should declare a variable, create an assignment, call a function or put a return");
  }
  printf("<<<<<StatementComponent");
  return c;
}

Statement* Parser::statement()
{
  Statement* s = new Statement();
  if(lookahead.lexeme == "IF TOBY IS NOT HERE and"){
    s->i = ifstatement();
  }
  else if(lookahead.lexeme == "FOR EACH ANGELAS CAT"){
    s->f = forstatement();
  }
  else if(lookahead.lexeme == "WHILE STANLEY IS SLEEPING and"){
    s->w = whilestatement();
  }
  else if(lookahead.lexeme == "RYAN DOES"){
    s->d = dowhilestatement();
  }
  advance();
  printf("<<<<<Statement");
  return s;
}

ElseIfStatement* Parser::elseifstatement()
{
  ElseIfStatement* ei = new ElseIfStatement();
  match("TOBY IS HERE OH WAIT IT WASNT HIM SO IF");
  advance();
  ei->l = logic();
  match(":");
  ei->c = ifstatementcomponents();
  printf("<<<<<ElseIfStatement");
  return ei;
}


ElseIfStatements* Parser::elseifstatements()
{
  ElseIfStatements* ei = new ElseIfStatements();
	while(true){
		ei->add(elseifstatement());
		if(lookahead.lexeme != "TOBY IS HERE OH WAIT IT WASNT HIM SO IF"){
      printf("<<<<<ElseIfStatements");
			return ei;
    }
	}
  return ei;
}

ElseStatement* Parser::elsestatement()
{
  ElseStatement* e = new ElseStatement();
  match("TOBY IS COMING");
  advance();
  e->c = ifstatementcomponents();
  printf("<<<<<ElseStatement");
  return e;
}

IfStatement* Parser::ifstatement()
{
  IfStatement* i = new IfStatement();
  advance();
  i->l = logic();
  match(":");
  i->c = ifstatementcomponents();
  if(lookahead.lexeme == "TOBY IS HERE OH WAIT IT WASNT HIM SO IF")
    i->ei = elseifstatements();
  if(lookahead.lexeme == "TOBY IS COMING")
    i->e = elsestatement();
  match("TOBY ARRIVED");
  printf("<<<<<IfStatement");
  return i;
}

ForStatement* Parser::forstatement()
{
  ForStatement* f = new ForStatement();
  match("FOR EACH ANGELAS CAT");
  f->variables = assignments();
  match(":");
  advance();
  f->l = logic();
  match(":");
  f->o = assignments();
  match(":");
  f->c = forstatementcomponents();
  match("DWIGHT HAVE KILLED THEM");
  printf("<<<<<ForStatement");
  return f;
}

WhileStatement* Parser::whilestatement()
{
  WhileStatement* w = new WhileStatement();
  match("WHILE STANLEY IS SLEEPING and");
  advance();
  w->l = logic();
  match(":");
  w->c = whilestatementcomponents();
  match("THE WORKING DAY IS OVER");
  printf("<<<<<WhileStatement");
  return w;
}

DoWhileStatement* Parser::dowhilestatement()
{
  DoWhileStatement* d = new DoWhileStatement();
  match("RYAN DOES");
  advance();
  match(":");
  d->c = dowhilestatementcomponents();
  match("WHILE KELLY IS TALKING and");
  advance();
  d->l = logic();
  match(":");
  printf("<<<<<DoWhileStatement");
  return d;
}

Number* Parser::number()
{
  Number* n = new Number();
  n->val = std::stod(lookahead.lexeme);
  printf("<<<<<Number");
  return n;
}

MichaelScottExpressionNode* Parser::expressionPrecedence(int precedence,
                                      MichaelScottExpressionNode* LHS) {
  while (true) {
    if(lookahead.type != Token::tok_mathop) return LHS;
    int current_precedence = mathopPrecedence();
    if(current_precedence < precedence)
      return LHS;
    std::string current_operation = lookahead.lexeme;
    advance(); 
    MichaelScottExpressionNode* RHS = expressionprimary();
    if (!RHS) return nullptr;
    advance(); 
    if(lookahead.type != Token::tok_mathop) return new Expression(LHS, current_operation, RHS);
    int next_precedence = mathopPrecedence();
    if(current_precedence < next_precedence) {
      RHS = expressionPrecedence(current_precedence + 1, RHS);
      if(!RHS) return nullptr;
    }
    LHS = new Expression(LHS, current_operation, RHS);
  }
}

MichaelScottExpressionNode* Parser::expressionprimary()
{
  if(lookahead.type == Token::tok_variable)
    return variable("");
  else if(lookahead.type == Token::tok_number)
    return number();
  else if(lookahead.lexeme == "("){
    advance();
    MichaelScottExpressionNode* e = expression();
    match(")");
    return e;
  }
  error("You should declare a expression using numbers or variables");
  return nullptr;
}

MichaelScottExpressionNode* Parser::expression()
{
  MichaelScottExpressionNode* LHS = expressionprimary();
  advance();
  return expressionPrecedence(0, LHS);
}


LogicExpression* Parser::logicexpression()
{
  static int i =0;
  i++;
  LogicExpression* le = new LogicExpression();
  if(lookahead.type == Token::tok_notop){
    le->notLHS = true;
    advance();
  }
  if(lookahead.type == Token::tok_variable
    || lookahead.type == Token::tok_number)
    le->LHS = expression();
  else
    error("A variable or logic expression is expected");
  if(lookahead.type == Token::tok_compop)
    le->op = lookahead.lexeme;
  else
    error("A comparison operation is expected");
  advance();
  if(lookahead.type == Token::tok_notop){
    le->notRHS = true;
    advance();
  }
  if(lookahead.type == Token::tok_variable
    || lookahead.type == Token::tok_number)
    le->RHS = expression();
  else
    error("A variable or logic expression is expected");
  return le;
}


Logic* Parser::logicPrecedence(int precedence,
                                      Logic* LHS) {
  static int i =0;
  i++;
  int previous_precedence = -1;
  while (true) {
    if(lookahead.type != Token::tok_logicop) return LHS;
    int current_precedence = logicopPrecedence();
    if(current_precedence < precedence) return LHS;
    std::string current_operation = lookahead.lexeme;
    advance(); 
    Logic* RHS = new Logic();
    RHS->LHS = logicprimary();
    if(!RHS->LHS) return nullptr;
    if(lookahead.type != Token::tok_logicop){
      if(previous_precedence > current_precedence)
        LHS->addRHS(current_operation, RHS->LHS, true);
      else
        LHS->addRHS(current_operation, RHS->LHS, false);
      return LHS;
    }
    previous_precedence = current_precedence;
    int next_precedence = logicopPrecedence();
    if(current_precedence < next_precedence) {
      RHS = logicPrecedence(current_precedence + 1, RHS);
      if(!RHS) return nullptr;
    }
    if(!LHS->RHS && !RHS->RHS) 
      LHS = new Logic(LHS->LHS, current_operation, RHS->LHS);
    else if(!LHS->RHS) 
      LHS = new Logic(LHS->LHS, current_operation, RHS);
    else if(!RHS->RHS)
      LHS = new Logic(LHS, current_operation, RHS->LHS);
    else
      LHS = new Logic(LHS, current_operation, RHS);
  }
}

MichaelScottLogicExpressionNode* Parser::logicprimary()
{
  if(lookahead.type == Token::tok_variable
    || lookahead.type == Token::tok_number
    || lookahead.type == Token::tok_notop){
    return logicexpression();
  }
  else if(lookahead.lexeme == "("){
    advance();
    Logic* le = logic();
    match(")");
    advance();
    return le;
  }
  error("You should declare a expression using numbers or variables");
  return nullptr;
}

Logic* Parser::logic()
{
  static int i =0;
  i++;
  Logic* l = new Logic();
  l->LHS = logicprimary();
  return logicPrecedence(0, l);
}


Assignment* Parser::assignment()
{
  Assignment* a = new Assignment();
  bool parenthesis;
  a->v = variable("");
  advance();
  match("=");
  advance();
  if(lookahead.lexeme == "THATS WHAT SHE SAID"){
    a->f = functioncall();
  }
  else if(lookahead.type == Token::tok_variable
          || lookahead.type == Token::tok_number){
    a->e = (Expression*) expression();
    printf("======== %s\n", a->e->print().c_str());
  }
  else
    error("You should assign something to the variable");
  match("SOLD");
  advance();
  printf("<<<<<Assignment");
  return a;
}

Assignment* Parser::assignmentitem()
{
  Assignment* a = new Assignment();
  bool parenthesis;
  a->v = variable("");
  advance();
  match("=");
  advance();
  if(lookahead.lexeme == "THATS WHAT SHE SAID"){
    a->f = functioncall();
  }
  else if(lookahead.type == Token::tok_variable
          || lookahead.type == Token::tok_number){
    a->e = (Expression*) expression();
    printf("======== %s\n", a->e->print().c_str());
  }
  else
    error("You should assign something to the variable");
  printf("<<<<<Assignment");
  return a;
}

Assignments* Parser::assignments()
{
  Assignments* a = new Assignments();
  while(true) {
    advance();
		if(lookahead.lexeme == ":") return a; 
		a->add(assignmentitem());
		if (lookahead.lexeme != ","){
      printf("<<<<<Assignments");
      return a;
    }
	}
  return a;
}
