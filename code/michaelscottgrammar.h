#ifndef __michaelscottgrammar_h
#define __michaelscottgrammar_h

#include "ast.h"
#include "llvm.h"

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

static std::map<std::string, llvm::Value*> LLVMNamedValues;
static std::map<std::string, llvm::AllocaInst*> LLVMVariables;
static std::map<std::string, std::unique_ptr<Function>> LLVMFunctions;

llvm::Function *getFunction(std::string name) {
  // First, see if the function has already been added to the current module.
  if (auto *F = LLVMModule->getFunction(name))
    return F;

  // If not, check whether we can codegen the declaration from some existing
  // prototype.
  /*auto FI = LLVMFunctions.find(name);
  if (FI != LLVMFunctions.end())
    return (llvm::Function *) FI->second;//->codegen();
*/
  // If no existing prototype exists, return null.
  return nullptr;
}



class Variable: public MichaelScottNode, public MichaelScottExpressionNode{
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
  llvm::Value *codegen() {
    std::cout << "VARIABLE" << std::endl;

    return nullptr;// LLVMBuilder->CreateStore(llvm::HalfTyID);
  };
};

class Variables: public MichaelScottNode, public List<Variable>{
public:
  Variables(){
    printf(">>>>>Variables\n");
  }
  llvm::Value *codegen() {
    std::cout << "VARIABLESSSS" << std::endl;
    return nullptr;
  };
};

class VariableDeclaration: public MichaelScottNode{
public:
  std::string type;
  Variables *v;
  VariableDeclaration(){
    printf(">>>>>VariableDeclaration\n");
  }
  llvm::Value *codegen() {
    std::cout << "VARIABLE DECLARATION" << std::endl;
    llvm::Function *F = LLVMBuilder->GetInsertBlock()->getParent();
    llvm::Type *T = getTypeLLVM(this->type);
    llvm::Value *init = LLVMValuesInits[this->type];
    for (auto i = v->iterator(); i->hasNext();){
      Variable *var = (Variable *) i->next();
      llvm::AllocaInst *Alloca = createEntryBlockAlloca(F, var->name, T);
      LLVMBuilder->CreateStore(init, Alloca);
    }
    return nullptr;
  };
};

class Number: public MichaelScottExpressionNode {
public:
  double val;
  Number() {
    printf(">>>>>Number\n");
  }
  Number(double val):val(val) {}
  std::string print(){
    return std::to_string(val);
  }
  llvm::Value *codegen() {
    std::cout << "NUMBER" << std::endl;
    return llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(val));
  };
};

class Expression: public MichaelScottExpressionNode {
public:
  MichaelScottExpressionNode *LHS;
  std::string op;
  MichaelScottExpressionNode *RHS;
  Expression() {
    printf(">>>>>Expression\n");
  }
  Expression(MichaelScottExpressionNode* LHS, std::string op, MichaelScottExpressionNode* RHS)
  : LHS(std::move(LHS)), op(move(op)), RHS(std::move(RHS)) {}
  std::string print(){
    std::string lhs = "";
    std::string rhs = ""; 
    if(LHS) lhs = LHS->print(); 
    if(RHS) rhs = RHS->print();
    return ">"+lhs+"< "+op+" >"+rhs+"< ";
  }
  llvm::Value *codegen() {
    std::cout << "EXPRESSION" << std::endl;
    if(LHS) LHS->codegen()->print(llvm::errs());
    if(RHS) RHS->codegen()->print(llvm::errs());
    return nullptr;
  };
};

class LogicExpression: public MichaelScottLogicExpressionNode {
public:
  bool notLHS;
  MichaelScottExpressionNode *LHS;
  std::string op;
  bool notRHS;
  MichaelScottExpressionNode *RHS;
  LogicExpression() {
    notLHS = false;
    notRHS = false;
    printf(">>>>>LogicExpression\n");
  }
  LogicExpression(MichaelScottExpressionNode* LHS, std::string op, MichaelScottExpressionNode* RHS)
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
  llvm::Value *codegen() {return nullptr;};
};

class Logic: public MichaelScottLogicExpressionNode {
public:
  MichaelScottLogicExpressionNode *LHS;
  std::string op;
  MichaelScottLogicExpressionNode *RHS;
  Logic() {
    printf(">>>>>Logic\n");
  }
  Logic(MichaelScottLogicExpressionNode* LHS): LHS(std::move(LHS)) {}
  Logic(MichaelScottLogicExpressionNode* LHS, std::string op, MichaelScottLogicExpressionNode* RHS)
  : LHS(std::move(LHS)), op(move(op)), RHS(std::move(RHS)) {}
  std::string print(){
    std::string lhs = "";
    std::string rhs = "";
    if(LHS) lhs = LHS->print();
    if(RHS) rhs = RHS->print();
    return "("+lhs+") "+op+" ("+rhs+") ";
  }
  
  void addRHS(std::string operation, MichaelScottLogicExpressionNode* newRHS, bool lowerprecedence){
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
  llvm::Value *codegen() {return nullptr;};
};

class Include: public MichaelScottNode{
public:
  std::string library;
  Include(){
    printf(">>>>>Include\n");
  }
  llvm::Value *codegen() {
    std::cout<< "INCLUDE" << std::endl;
    return nullptr;
  };
};

class Argument: public MichaelScottNode{
public:
  std::string name;
  Argument(){
    printf(">>>>>Argument\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class Arguments: public MichaelScottNode, public List<Argument>{
public:
  Arguments(){
    printf(">>>>>Arguments\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class FunctionCall: public MichaelScottNode{
public:
  std::string name;
  Arguments *args;
  FunctionCall(){
    printf(">>>>>FunctionCall\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class Assignment: public MichaelScottNode{
public:
  Variable *v;
  Expression *e;
  FunctionCall *f;
  Assignment(){
    e, f = NULL;
    printf(">>>>>Assignment\n");
  }
  llvm::Value *codegen() {
    //verify if variable exists
    std::cout<< "ASSIGNMENT" << std::endl;
    v->codegen();
    if(e) {
      e->codegen();
    }
    else if(f) f->codegen();

    return nullptr;
  };
};

class Assignments: public MichaelScottNode, public List<Assignment>{
public:
  Assignments(){
    printf(">>>>>Assignments\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class Parameter: public MichaelScottNode{
public:
  std::string type;
  std::string name;
  Parameter(){
    printf(">>>>>Parameter\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class Parameters: public MichaelScottNode, public List<Parameter>{
public:
  Parameters(){
    printf(">>>>>Parameters\n");
  }
  llvm::Value *codegen() {



    return nullptr;
  };
};

class FunctionReturn: public MichaelScottNode{
public:
  std::string v;
  FunctionReturn(){
    printf(">>>>>FunctionReturn\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class StatementComponents;

class ElseStatement: public MichaelScottNode{
public:
  StatementComponents *c;
  ElseStatement(){
    printf(">>>>>ElseStatement\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class ElseIfStatement: public MichaelScottNode{
public:
  Logic *l;
  StatementComponents *c;
  ElseIfStatement(){
    printf(">>>>>ElseIfStatement\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class ElseIfStatements: public MichaelScottNode, public List<ElseIfStatement>{
public:
  ElseIfStatements(){
    printf(">>>>>ElseIfStatements\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class IfStatement: public MichaelScottNode{
public:
  Logic *l;
  StatementComponents *c;
  ElseIfStatements *ei;
  ElseStatement *e;
  IfStatement(){
    printf(">>>>>IfStatement\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class ForStatement: public MichaelScottNode{
public:
  Assignments *variables;
  Logic *l;
  Assignments *o;
  StatementComponents *c;
  ForStatement(){
    printf(">>>>>ForStatement\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class WhileStatement: public MichaelScottNode{
public:
  Logic *l;
  StatementComponents *c;
  WhileStatement(){
    printf(">>>>>WhileStatement\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class DoWhileStatement: public MichaelScottNode{
public:
  Logic *l;
  StatementComponents *c;
  DoWhileStatement(){
    printf(">>>>>DoWhileStatement\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class Statement: public MichaelScottNode{
public:
  IfStatement *i;
  ForStatement *f;
  WhileStatement *w;
  DoWhileStatement *d;
  Statement(){
    printf(">>>>>Statement\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class StatementComponent: public MichaelScottNode{
public:
  FunctionCall *fc;
  FunctionReturn *fr;
  VariableDeclaration *vd;
  Statement *s;
  Assignment *a;
  StatementComponent(){
    printf(">>>>>StatementComponent\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class StatementComponents: public MichaelScottNode, public List<StatementComponent>{
public:
  StatementComponents(){
    printf(">>>>>StatementComponents\n");
  }
  llvm::Value *codegen() {return nullptr;};
};

class FunctionComponent: public MichaelScottNode{
public:
  FunctionCall *fc;
  FunctionReturn *fr;
  VariableDeclaration *vd;
  Statement *s;
  Assignment *a;
  FunctionComponent(){
    fc, fr, vd, s, a = NULL;
    printf(">>>>>FunctionComponent\n");
  }
  llvm::Value *codegen() {
    std::cout<< "FUNCTION COMPONENT" << std::endl;
    if(fc) fc->codegen();
    else if(fr) fr->codegen();
    else if(vd) vd->codegen();
    else if(s) s->codegen();
    else if(a) a->codegen();
    return nullptr;
  };
};

class FunctionComponents: public MichaelScottNode, public List<FunctionComponent>{
public:
  FunctionComponents(){
    printf(">>>>>FunctionComponents\n");
  }
  llvm::Value *codegen() {
    for (auto i = this->iterator(); i->hasNext();)
      i->next()->codegen();
    return nullptr;
  };
};

class Function: public MichaelScottNode{
public:
  std::string name;
  std::string return_type;
  Parameters *p;
  FunctionComponents *body;
  
  Function(){
    printf(">>>>>Function\n");
    return_type = "void";
  }
  llvm::Value *codegen() {
    std::cout<< "FUNCTION" << std::endl;

    std::vector<llvm::Type *> Doubles(p->size(), llvm::Type::getDoubleTy(*LLVMContext));
    std::vector<llvm::Type *> ptypes;
    std::vector<std::string> pnames;


    for (auto i = p->iterator(); i->hasNext();){
      Parameter *par = i->next(); 
      std::string type = par->type;
      std::string name = par->name;
      pnames.push_back(name);
      ptypes.push_back(getTypeLLVM(type));
    }


    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*LLVMContext), 
                                                     ptypes, false);

    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, 
                                               this->name, LLVMModule.get());
   
    std::cout << "F" << F << std::endl;
    // Set names for all arguments.
    unsigned idx = 0;
    for (auto &arg : F->args()){
      arg.setName(pnames[idx++]);
    }    
    

    llvm::BasicBlock *BBentry = llvm::BasicBlock::Create(*LLVMContext, "entry", F);
    LLVMBuilder->SetInsertPoint(BBentry);
    std::cout << "F" << BBentry << std::endl;
    F->print(llvm::errs());
    

    LLVMNamedValues.clear();
    for (auto &Arg : F->args()) {
      // Create an alloca for this variable.
      llvm::AllocaInst *Alloca = createEntryBlockAlloca(F, std::string(Arg.getName())+"_var", Arg.getType());

      // Store the initial value into the alloca.
      LLVMBuilder->CreateStore(&Arg, Alloca);

      // Add arguments to variable symbol table.
      LLVMNamedValues[std::string(Arg.getName())] = Alloca;
    }
    for (auto it = LLVMNamedValues.begin(); it != LLVMNamedValues.end(); it++)
    {
        std::cout << it->first    // string (key)
              << ':'
              << it->second   // string's value 
              << std::endl; 
    }


    this->body->codegen();


    //LLVMBuilder->SetInsertPoint(BBbody);
    F->print(llvm::errs());
    return F;
    if (llvm::Value *RetVal = this->body->codegen()) {
      // Finish off the function.
      LLVMBuilder->CreateRet(RetVal);

      // Validate the generated code, checking for consistency.
      llvm::verifyFunction(*F);

      // Run the optimizer on the function.
      LLVMFPM->run(*F);


      F->print(llvm::errs());
    }

    if (llvm::Value *RetVal = this->body->codegen()) {
      // Finish off the function.
      LLVMBuilder->CreateRet(RetVal);

      // Validate the generated code, checking for consistency.
      llvm::verifyFunction(*F);

      // Run the optimizer on the function.
      LLVMFPM->run(*F);


      F->print(llvm::errs());
      return F;
    }
  
    // Error reading body, remove function.
    F->eraseFromParent();

    return nullptr;
  };
};

class GlobalVariableDeclaration: public MichaelScottNode{
public:
  VariableDeclaration* vd;
  GlobalVariableDeclaration(){
    printf(">>>>>GlobalVariableDeclaration\n");
  }
  llvm::Value *codegen() {
    std::cout<< "GLOBAL VARIABLE DECLARATION" << std::endl;
    return nullptr;
  };
};

class Component: public MichaelScottNode{
public:
  Include *i;
  GlobalVariableDeclaration *gvd;
  Function *f;
  Component(){
    i = NULL;
    gvd = NULL;
    f = NULL;
    printf(">>>>>Component\n");
  }
  llvm::Value *codegen() {
    std::cout<< "COMPONENT" << std::endl;
    if(i) i->codegen();
    else if(gvd) gvd->codegen();
    else if(f) f->codegen();
    return nullptr;
  };
};

class Components: public MichaelScottNode, public List<Component>{
public:
  Components(){
    printf(">>>>>Components\n");
  }
  llvm::Value *codegen() {
    std::cout<< "HI" << std::endl;
    //for(it = this->iterator(); this->iterator().hasNext(); this)
    for (IteratorPtr i = this->iterator(); i->hasNext();)
      i->next()->codegen();
    std::cout<< "BYE" << std::endl;
    return nullptr;
  };
};

class MichaelScott: public MichaelScottNode{
public:
  Components *c;
  MichaelScott(){
    printf(">>>>>MichaelScott\n");
  }
  llvm::Value *codegen() {
    InitializeModule();
    std::cout<< std::endl;
    return c->codegen();
  };
};

#endif // __michaelscottgrammar_h