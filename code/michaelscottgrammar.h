#ifndef __michaelscottgrammar_h
#define __michaelscottgrammar_h

#include "main.h"
#include "grammar.h"

class Variable: public Node, public ExpressionNode{
public:
  std::string type;
  std::string name;
  Variable(){
    printf(">>>>>Variable\n");
  }
  Variable(std::string type):type(move(type)){
    printf(">>>>>Variable\n");
  }
  Variable(std::string type, std::string name):
              type(move(type)), name(move(name)){
    printf(">>>>>Variable\n");
  }
  std::string print(){
    return name;
  }
};

class Variables: public Node, public List<Variable>{
public:
  Variables(){
    printf(">>>>>Variables\n");
  }
};

class VariableDeclaration: public Node{
public:
  std::string type;
  Variables *v;
  VariableDeclaration(){
    printf(">>>>>VariableDeclaration\n");
  }
};

class Number: public ExpressionNode {
public:
  double val;
  Number() {
    printf(">>>>>Number\n");
  }
  Number(double val):val(val) {}
  std::string print(){
    return std::to_string(val);
  }
};

class Expression: public ExpressionNode {
public:
  ExpressionNode *LHS;
  std::string op;
  ExpressionNode *RHS;
  Expression() {
    printf(">>>>>Expression\n");
  }
  Expression(ExpressionNode* LHS, std::string op, ExpressionNode* RHS)
  : LHS(std::move(LHS)), op(move(op)), RHS(std::move(RHS)) {}
  std::string print(){
    std::string lhs = "";
    std::string rhs = ""; 
    if(LHS) lhs = LHS->print(); 
    if(RHS) rhs = RHS->print();
    return ">"+lhs+"< "+op+" >"+rhs+"< ";
  }
};


class LogicExpression: public LogicExpressionNode {
public:
  bool notLHS;
  ExpressionNode *LHS;
  std::string op;
  bool notRHS;
  ExpressionNode *RHS;
  LogicExpression() {
    notLHS = false;
    notRHS = false;
    printf(">>>>>LogicExpression\n");
  }
  LogicExpression(ExpressionNode* LHS, std::string op, ExpressionNode* RHS)
  : LHS(std::move(LHS)), op(move(op)), RHS(std::move(RHS)) {
    notLHS = false;
    notRHS = false;
  }
  std::string print(){
    std::string notlhs = "";
    std::string lhs = "";
    std::string notrhs = "";
    std::string rhs = "";
    if(notLHS) notlhs = "not"; 
    if(LHS) lhs = LHS->print(); 
    if(notRHS) notrhs = "not"; 
    if(RHS) rhs = RHS->print();
    return "("+notlhs+lhs+") "+op+" ("+notrhs+rhs+") ";
  }
  int getRHSType(){ return 1; };
};

class Logic: public LogicExpressionNode {
public:
  LogicExpressionNode *LHS;
  std::string op;
  LogicExpressionNode *RHS;
  Logic() {
    printf(">>>>>Logic\n");
  }
  Logic(LogicExpressionNode* LHS): LHS(std::move(LHS)) {}
  Logic(LogicExpressionNode* LHS, std::string op, LogicExpressionNode* RHS)
  : LHS(std::move(LHS)), op(move(op)), RHS(std::move(RHS)) {}
  std::string print(){
    std::string lhs = "";
    std::string rhs = "";
    if(LHS) lhs = LHS->print();
    if(RHS) rhs = RHS->print();
    return "("+lhs+") "+op+" ("+rhs+") ";
  }
  void addRHS(std::string operation, LogicExpressionNode* newRHS, bool lowerprecedence){
    Logic* auxRHS;
    Logic* prevRHS;
    bool changed = false;
    auxRHS = this;
    while(!changed){
      if(auxRHS->RHS == NULL){
        auxRHS->RHS = newRHS;
        auxRHS->op = operation;
        changed = true;
      }
      else if(auxRHS->RHS->getRHSType() == 1){
        Logic* newLogic = new Logic();
        if(auxRHS != this){
          if(lowerprecedence){
            newLogic->LHS = auxRHS->RHS;
            newLogic->RHS = newRHS;
            newLogic->op = operation;
            auxRHS->RHS = newLogic;
          }
          else{
            newLogic->LHS = auxRHS;
            newLogic->RHS = newRHS;
            newLogic->op = operation;
            prevRHS->RHS = newLogic;
          }
        }
        else{
          if(lowerprecedence){
            newLogic->LHS = this->LHS;
            newLogic->RHS = this->RHS;
            newLogic->op = this->op;
            this->LHS = newLogic;
            this->RHS = newRHS;
            this->op = operation;
          }
          else{
            newLogic->LHS = this->RHS;
            newLogic->RHS = newRHS;
            newLogic->op = operation;
            this->RHS = newLogic;
          }
        }
        changed = true;
      }
      else{
        prevRHS = auxRHS;
        auxRHS= (Logic*) auxRHS->RHS;
      }
    };
  }
  int getRHSType(){ return 2; };
};

class Include: public Node{
public:
  std::string library;
  Include(){
    printf(">>>>>Include\n");
  }
};

class FunctionCall;

class Assignment: public Node{
public:
  Expression* e;
  Variable *v;
  FunctionCall *f;
  Assignment(){
    printf(">>>>>Assignment\n");
  }
};

class Assignments: public Node, public List<Assignment>{
public:
  Assignments(){
    printf(">>>>>Assignments\n");
  }
};

class Parameter: public Node{
public:
  std::string type;
  std::string name;
  Parameter(){
    printf(">>>>>Parameter\n");
  }
};

class Parameters: public Node, public List<Parameter>{
public:
  Parameters(){
    printf(">>>>>Parameters\n");
  }
};

class Argument: public Node{
public:
  std::string name;
  Argument(){
    printf(">>>>>Argument\n");
  }
};

class Arguments: public Node, public List<Argument>{
public:
  Arguments(){
    printf(">>>>>Arguments\n");
  }
};

class FunctionCall: public Node{
public:
  std::string name;
  Arguments *args;
  FunctionCall(){
    printf(">>>>>FunctionCall\n");
  }
};

class FunctionReturn: public Node{
public:
  std::string v;
  FunctionReturn(){
    printf(">>>>>FunctionReturn\n");
  }
};

class StatementComponents;

class ElseStatement: public Node{
public:
  StatementComponents *c;
  ElseStatement(){
    printf(">>>>>ElseStatement\n");
  }
};

class ElseIfStatement: public Node{
public:
  Logic *l;
  StatementComponents *c;
  ElseIfStatement(){
    printf(">>>>>ElseIfStatement\n");
  }
};

class ElseIfStatements: public Node, public List<ElseIfStatement>{
public:
  ElseIfStatements(){
    printf(">>>>>ElseIfStatements\n");
  }
};

class IfStatement: public Node{
public:
  Logic *l;
  StatementComponents *c;
  ElseIfStatements *ei;
  ElseStatement *e;
  IfStatement(){
    printf(">>>>>IfStatement\n");
  }
};

class ForStatement: public Node{
public:
  Assignments *variables;
  Logic *l;
  Assignments *o;
  StatementComponents *c;
  ForStatement(){
    printf(">>>>>ForStatement\n");
  }
};

class WhileStatement: public Node{
public:
  Logic *l;
  StatementComponents *c;
  WhileStatement(){
    printf(">>>>>WhileStatement\n");
  }
};

class DoWhileStatement: public Node{
public:
  Logic *l;
  StatementComponents *c;
  DoWhileStatement(){
    printf(">>>>>DoWhileStatement\n");
  }
};

class Statement: public Node{
public:
  IfStatement *i;
  ForStatement *f;
  WhileStatement *w;
  DoWhileStatement *d;
  Statement(){
    printf(">>>>>Statement\n");
  }
};


class FunctionComponent: public Node{
public:
  FunctionCall *fc;
  FunctionReturn *fr;
  VariableDeclaration *vd;
  Statement *s;
  Assignment *a;
  FunctionComponent(){
    printf(">>>>>FunctionComponent\n");
  }
};

class FunctionComponents: public Node, public List<FunctionComponent>{
public:
  FunctionComponents(){
    printf(">>>>>FunctionComponents\n");
  }
};

class StatementComponent: public Node{
public:
  FunctionCall *fc;
  FunctionReturn *fr;
  VariableDeclaration *vd;
  Statement *s;
  Assignment *a;
  StatementComponent(){
    printf(">>>>>StatementComponent\n");
  }
};

class StatementComponents: public Node, public List<StatementComponent>{
public:
  StatementComponents(){
    printf(">>>>>StatementComponents\n");
  }
};

class Function: public Node{
public:
  std::string name;
  Parameters *p;
  FunctionComponents *body;
  Function(){
    printf(">>>>>Function\n");
  }
};

class GlobalVariableDeclaration: public Node{
public:
  VariableDeclaration* vd;
  GlobalVariableDeclaration(){
    printf(">>>>>GlobalVariableDeclaration\n");
  }
};

class Component: public Node{
public:
  Include *i;
  GlobalVariableDeclaration *gvd;
  Function *f;
  Component(){
    printf(">>>>>Component\n");
  }
};

class Components: public Node, public List<Component>{
public:
  Components(){
    printf(">>>>>Components\n");
  }
};

class MichaelScott: public Node{
public:
  Components *c;
  MichaelScott(){
    printf(">>>>>MichaelScott\n");
  }
};
#endif // __michaelscottgrammar_h