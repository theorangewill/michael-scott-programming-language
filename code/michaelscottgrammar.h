#ifndef __michaelscottgrammar_h
#define __michaelscottgrammar_h
#ifndef __iostream
    #define __iostream
    #include <iostream>
#endif

#include "ast.h"
#include "llvm.h"

static std::map<std::string, llvm::Value*> LLVMNamedValues;
static std::map<std::string, llvm::AllocaInst*> LLVMVariables;
//nao sei qual function
static std::map<std::string, std::unique_ptr<llvm::Function>> LLVMFunctions;

llvm::Function *getFunction(std::string name);

class MichaelScottNode: public Node{
public:
  virtual llvm::Value *codegen() = 0;
};

class MichaelScottExpressionNode: public ExpressionNode{
public:
  virtual llvm::Value *codegen() = 0;
};

class MichaelScottLogicExpressionNode: public LogicExpressionNode{
public:
  virtual llvm::Value *codegen() = 0;
};

class Function;


class Variable: public MichaelScottNode, public MichaelScottExpressionNode{
public:
  std::string type;
  std::string name;
  Variable();
  Variable(std::string type);
  Variable(std::string type, std::string name);
  std::string print();
  llvm::Value *codegen();
};

class Variables: public MichaelScottNode, public List<Variable>{
public:
  Variables();
  llvm::Value *codegen();
};

class VariableDeclaration: public MichaelScottNode{
public:
  std::string type;
  Variables *v;
  VariableDeclaration();
  llvm::Value *codegen();
};

class Number: public MichaelScottExpressionNode {
public:
  double val;
  Number();
  Number(double val);
  std::string print();
  llvm::Value *codegen();
};

class Expression: public MichaelScottExpressionNode {
public:
  MichaelScottExpressionNode *LHS;
  std::string op;
  MichaelScottExpressionNode *RHS;
  Expression();
  Expression(MichaelScottExpressionNode* LHS, std::string op, MichaelScottExpressionNode* RHS);
  std::string print();
  llvm::Value *codegen();
};

class LogicExpression: public MichaelScottLogicExpressionNode {
public:
  bool notLHS;
  MichaelScottExpressionNode *LHS;
  std::string op;
  bool notRHS;
  MichaelScottExpressionNode *RHS;
  LogicExpression();
  LogicExpression(MichaelScottExpressionNode* LHS, std::string op, MichaelScottExpressionNode* RHS);
  std::string print();
  int getRHSType();
  llvm::Value *codegen();
};

class Logic: public MichaelScottLogicExpressionNode {
public:
  MichaelScottLogicExpressionNode *LHS;
  std::string op;
  MichaelScottLogicExpressionNode *RHS;
  Logic();
  Logic(MichaelScottLogicExpressionNode* LHS);
  Logic(MichaelScottLogicExpressionNode* LHS, std::string op, MichaelScottLogicExpressionNode* RHS);
  std::string print();
  void addRHS(std::string operation, MichaelScottLogicExpressionNode* newRHS, bool lowerprecedence);
  int getRHSType();
  llvm::Value *codegen();
};

class Include: public MichaelScottNode{
public:
  std::string library;
  Include();
  llvm::Value *codegen();
};

class Argument: public MichaelScottNode{
public:
  std::string name;
  Argument();
  llvm::Value *codegen();
};

class Arguments: public MichaelScottNode, public List<Argument>{
public:
  Arguments();
  llvm::Value *codegen();
};

class FunctionCall: public MichaelScottNode{
public:
  std::string name;
  Arguments *args;
  FunctionCall();
  llvm::Value *codegen();
};

class Assignment: public MichaelScottNode{
public:
  Variable *v;
  Expression *e;
  FunctionCall *f;
  Assignment();
  llvm::Value *codegen();
};

class Assignments: public MichaelScottNode, public List<Assignment>{
public:
  Assignments();
  llvm::Value *codegen();
};

class Parameter: public MichaelScottNode{
public:
  std::string type;
  std::string name;
  Parameter();
  llvm::Value *codegen();
};

class Parameters: public MichaelScottNode, public List<Parameter>{
public:
  Parameters();
  llvm::Value *codegen();
};

class FunctionReturn: public MichaelScottNode{
public:
  std::string v;
  FunctionReturn();
  llvm::Value *codegen();
};

class StatementComponents;

class ElseStatement: public MichaelScottNode{
public:
  StatementComponents *c;
  ElseStatement();
  llvm::Value *codegen();
};

class ElseIfStatement: public MichaelScottNode{
public:
  Logic *l;
  StatementComponents *c;
  ElseIfStatement();
  llvm::Value *codegen();
};

class ElseIfStatements: public MichaelScottNode, public List<ElseIfStatement>{
public:
  ElseIfStatements();
  llvm::Value *codegen();
};

class IfStatement: public MichaelScottNode{
public:
    Logic *l;
    StatementComponents *c;
    ElseIfStatements *ei;
    ElseStatement *e;
    IfStatement();
    llvm::Value *codegen();
  };

class ForStatement: public MichaelScottNode{
public:
  Assignments *variables;
  Logic *l;
  Assignments *o;
  StatementComponents *c;
  ForStatement();
  llvm::Value *codegen();
};

class WhileStatement: public MichaelScottNode{
public:
  Logic *l;
  StatementComponents *c;
  WhileStatement();
  llvm::Value *codegen();
};

class DoWhileStatement: public MichaelScottNode{
public:
  Logic *l;
  StatementComponents *c;
  DoWhileStatement();
  llvm::Value *codegen();
};

class Statement: public MichaelScottNode{
public:
  IfStatement *i;
  ForStatement *f;
  WhileStatement *w;
  DoWhileStatement *d;
  Statement();
  llvm::Value *codegen();
};

class StatementComponent: public MichaelScottNode{
public:
  FunctionCall *fc;
  FunctionReturn *fr;
  VariableDeclaration *vd;
  Statement *s;
  Assignment *a;
  StatementComponent();
  llvm::Value *codegen();
};

class StatementComponents: public MichaelScottNode, public List<StatementComponent>{
public:
  StatementComponents();
  llvm::Value *codegen();
};

class FunctionComponent: public MichaelScottNode{
public:
  FunctionCall *fc;
  FunctionReturn *fr;
  VariableDeclaration *vd;
  Statement *s;
  Assignment *a;
  FunctionComponent();
  llvm::Value *codegen();
};

class FunctionComponents: public MichaelScottNode, public List<FunctionComponent>{
public:
  FunctionComponents();
  llvm::Value *codegen();
};

class Function: public MichaelScottNode{
public:
  std::string name;
  std::string return_type;
  Parameters *p;
  FunctionComponents *body;
  Function();
  llvm::Value *codegen();
};

class GlobalVariableDeclaration: public MichaelScottNode{
public:
  VariableDeclaration* vd;
  GlobalVariableDeclaration();
  llvm::Value *codegen();
};

class Component: public MichaelScottNode{
public:
  Include *i;
  GlobalVariableDeclaration *gvd;
  Function *f;
  Component();
  llvm::Value *codegen();
};

class Components: public MichaelScottNode, public List<Component>{
public:
  Components();
  llvm::Value *codegen();
};

class MichaelScott: public MichaelScottNode{
public:
  Components *c;
  MichaelScott();
  llvm::Value *codegen();
};

#endif // __michaelscottgrammar_h