#ifndef __parser_h
#define __parser_h

#include "lexic.h"
#include "michaelscottgrammar.h"

#ifndef __vector
  #define __vector
  #include <vector>
#endif

#ifndef __map
  #define __map
  #include <map>
#endif

class Parser{
public:
  MichaelScott* parse(const char*);
  Parser();
private:
  char* buffer;
  int lineNumber;
  Token lookahead;
  std::string IdentifierStr;
  std::map<std::string, int> MathopPrecedence;
  std::map<std::string, int> LogicopPrecedence;
  std::map<std::string, std::string> TypesDefinition;

  void error();
  void error(const char* error_warning);
  void error(const char* error_warning, std::string error_text);
  Token nextToken();
  int parserSupport(const char* support_text);

  void advance(){ 
    lookahead = nextToken(); 
    lookahead.print();
  }

  void match(int t)
  {
    if (lookahead.type != t)
      error();
  }
  void match(std::string l)
  {
    if (lookahead.lexeme != l)
      error(("Missing a "+l).c_str());
  }

  int mathopPrecedence() {
    if(lookahead.lexeme == "(" || lookahead.lexeme == ")")
      return -1;
    if(lookahead.type != Token::tok_mathop)
      error("A math operation is expected");
    int prec = MathopPrecedence[lookahead.lexeme];
    if (prec <= 0) return -1;
    return prec;
  }

  int logicopPrecedence() {
    if(lookahead.lexeme == "(" || lookahead.lexeme == ")")
      return -1;
    if(lookahead.type != Token::tok_logicop)
      error("A logic operation is expected");
    int prec = LogicopPrecedence[lookahead.lexeme];
    if (prec <= 0) return -1;
    return prec;
  }

  MichaelScott* michaelscott();
  Components* components();
  Component* component();
  Include* include();
  GlobalVariableDeclaration* globalvariabledeclaration();
  Function* function();
  Parameters* parameters();
  Parameter* parameter();
  FunctionComponents* functioncomponents();
  FunctionComponent* functioncomponent();
  Arguments* arguments();
  Argument* argument();
  FunctionCall* functioncall();
  FunctionReturn* functionreturn();
  VariableDeclaration* variabledeclaration();
  Variables* variables(std::string variable_type);
  Variable* variable(std::string variable_type);
  Variable* variable();
  StatementComponents* statementcomponents();
  StatementComponent* statementcomponent();
  StatementComponents* ifstatementcomponents();
  StatementComponent* ifstatementcomponent();
  StatementComponents* forstatementcomponents();
  StatementComponent* forstatementcomponent();
  StatementComponents* whilestatementcomponents();
  StatementComponent* whilestatementcomponent();
  StatementComponents* dowhilestatementcomponents();
  StatementComponent* dowhilestatementcomponent();
  Statement* statement();
  ElseIfStatements* elseifstatements();
  ElseIfStatement* elseifstatement();
  ElseStatement* elsestatement();
  IfStatement* ifstatement();
  ForStatement* forstatement();
  WhileStatement* whilestatement();
  DoWhileStatement* dowhilestatement();
  Assignments* assignments();
  Assignment* assignmentitem();
  Assignment* assignment();
  MichaelScottExpressionNode* expression();
  MichaelScottExpressionNode* expressionprimary();
  MichaelScottExpressionNode* expressionPrecedence(int precedence, MichaelScottExpressionNode* LHS);
  Number* number();
  LogicExpression* logicexpression();
  Logic* logic();
  MichaelScottLogicExpressionNode* logicprimary();
  Logic* logicPrecedence(int precedence, Logic* LHS);
}; // Parser

#endif // __parser_h