#include "michaelscottgrammar.h"

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

/*****************************************************************************/
/******************* Methods related to program ******************************/


MichaelScott::MichaelScott(){
  printf(">>>>>MichaelScott\n");
}

llvm::Value* MichaelScott::codegen() {
  InitializeModule();
  std::cout<< std::endl;
  return c->codegen();
}
Include::Include(){
  printf(">>>>>Include\n");
}

llvm::Value* Include::codegen() {
  std::cout<< "INCLUDE" << std::endl;
  return nullptr;
}

GlobalVariableDeclaration::GlobalVariableDeclaration(){
  printf(">>>>>GlobalVariableDeclaration\n");
}

llvm::Value* GlobalVariableDeclaration::codegen() {
  std::cout<< "GLOBAL VARIABLE DECLARATION" << std::endl;
  return nullptr;
}

Component::Component(){
  i = NULL;
  gvd = NULL;
  f = NULL;
  printf(">>>>>Component\n");
}

llvm::Value* Component::codegen() {
  std::cout<< "COMPONENT" << std::endl;
  if(i) i->codegen();
  else if(gvd) gvd->codegen();
  else if(f) f->codegen();
  return nullptr;
}

Components::Components(){
  printf(">>>>>Components\n");
}
llvm::Value* Components::codegen() {
  std::cout<< "HI" << std::endl;
  //for(it = this->iterator(); this->iterator().hasNext(); this)
  for (IteratorPtr i = this->iterator(); i->hasNext();)
    i->next()->codegen();
  std::cout<< "BYE" << std::endl;
  return nullptr;
}

/******************* Methods related to variables ****************************/

Variable::Variable(){
  printf(">>>>>Variable\n");
}

Variable::Variable(std::string type):type(move(type)){
  printf(">>>>>Variable\n");
}

Variable::Variable(std::string type, std::string name):
            type(move(type)), name(move(name)){
  printf(">>>>>Variable\n");
}

std::string Variable::print(){
  return name;
}

llvm::Value* Variable::codegen() {
  std::cout << "VARIABLE" << std::endl;

  return nullptr;// LLVMBuilder->CreateStore(llvm::HalfTyID);
}

Variables::Variables(){
  printf(">>>>>Variables\n");
}

llvm::Value* Variables::codegen() {
  std::cout << "VARIABLESSSS" << std::endl;
  return nullptr;
}

VariableDeclaration::VariableDeclaration(){
  printf(">>>>>VariableDeclaration\n");
}

llvm::Value* VariableDeclaration::codegen() {
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
}

Number::Number() {
  printf(">>>>>Number\n");
}

Number::Number(double val):val(val) {}
std::string Number::print(){
  return std::to_string(val);
}

llvm::Value* Number::codegen() {
  std::cout << "NUMBER" << std::endl;
  return llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(val));
}

/*****************************************************************************/
/******************* Methods related to expressions **************************/

Expression::Expression() {
  printf(">>>>>Expression\n");
}

Expression::Expression(MichaelScottExpressionNode* LHS, std::string op, MichaelScottExpressionNode* RHS)
: LHS(std::move(LHS)), op(move(op)), RHS(std::move(RHS)) {}

std::string Expression::print(){
  std::string lhs = "";
  std::string rhs = ""; 
  if(LHS) lhs = LHS->print(); 
  if(RHS) rhs = RHS->print();
  return ">"+lhs+"< "+op+" >"+rhs+"< ";
}

llvm::Value* Expression::codegen() {
  std::cout << "EXPRESSION" << std::endl;
  if(LHS) LHS->codegen()->print(llvm::errs());
  if(RHS) RHS->codegen()->print(llvm::errs());
  return nullptr;
}

LogicExpression::LogicExpression() {
  notLHS = false;
  notRHS = false;
  printf(">>>>>LogicExpression\n");
}

LogicExpression::LogicExpression(MichaelScottExpressionNode* LHS, std::string op, MichaelScottExpressionNode* RHS)
: LHS(std::move(LHS)), op(move(op)), RHS(std::move(RHS)) {
  notLHS = false;
  notRHS = false;
}

std::string LogicExpression::print(){
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

int LogicExpression::getRHSType(){ return 1; };
llvm::Value* LogicExpression::codegen() {
  return nullptr;
}

Logic::Logic() {
  printf(">>>>>Logic\n");
}

Logic::Logic(MichaelScottLogicExpressionNode* LHS): LHS(std::move(LHS)) {}

Logic::Logic(MichaelScottLogicExpressionNode* LHS, std::string op, MichaelScottLogicExpressionNode* RHS)
: LHS(std::move(LHS)), op(move(op)), RHS(std::move(RHS)) {}

std::string Logic::print(){
  std::string lhs = "";
  std::string rhs = "";
  if(LHS) lhs = LHS->print();
  if(RHS) rhs = RHS->print();
  return "("+lhs+") "+op+" ("+rhs+") ";
}

void Logic::addRHS(std::string operation, MichaelScottLogicExpressionNode* newRHS, bool lowerprecedence){
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

int Logic::getRHSType(){ return 2; };

llvm::Value* Logic::codegen() {
  return nullptr;
}

/*****************************************************************************/
/******************* Methods related to functions **************************/


Argument::Argument(){
  printf(">>>>>Argument\n");
}

llvm::Value* Argument::codegen() {
  return nullptr;
}

Arguments::Arguments(){
  printf(">>>>>Arguments\n");
}

llvm::Value* Arguments::codegen() {
  return nullptr;
}

FunctionCall::FunctionCall(){
  printf(">>>>>FunctionCall\n");
}

llvm::Value* FunctionCall::codegen() {
  return nullptr;
}

Assignment::Assignment(){
  e, f = NULL;
  printf(">>>>>Assignment\n");
}

llvm::Value* Assignment::codegen() {
  //verify if variable exists
  std::cout<< "ASSIGNMENT" << std::endl;
  v->codegen();
  if(e) {
    e->codegen();
  }
  else if(f) f->codegen();

  return nullptr;
}

Assignments::Assignments(){
  printf(">>>>>Assignments\n");
}

llvm::Value* Assignments::codegen() {
  return nullptr;
}

Parameter::Parameter(){
  printf(">>>>>Parameter\n");
}
llvm::Value* Parameter::codegen() {
  return nullptr;
}

Parameters::Parameters(){
  printf(">>>>>Parameters\n");
}

llvm::Value* Parameters::codegen() {
  return nullptr;
}

FunctionReturn::FunctionReturn(){
  printf(">>>>>FunctionReturn\n");
}

llvm::Value* FunctionReturn::codegen() {
  return nullptr;
}


FunctionComponent::FunctionComponent(){
  fc, fr, vd, s, a = NULL;
  printf(">>>>>FunctionComponent\n");
}
llvm::Value* FunctionComponent::codegen() {
  std::cout<< "FUNCTION COMPONENT" << std::endl;
  if(fc) fc->codegen();
  else if(fr) fr->codegen();
  else if(vd) vd->codegen();
  else if(s) s->codegen();
  else if(a) a->codegen();
  return nullptr;
}

FunctionComponents::FunctionComponents(){
  printf(">>>>>FunctionComponents\n");
}

llvm::Value* FunctionComponents::codegen() {
  for (auto i = this->iterator(); i->hasNext();)
    i->next()->codegen();
  return nullptr;
}

Function::Function(){
  printf(">>>>>Function\n");
  return_type = "void";
}

llvm::Value* Function::codegen() {
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
}


/*****************************************************************************/
/******************* Methods related to statements ***************************/

ElseStatement::ElseStatement(){
  printf(">>>>>ElseStatement\n");
}

llvm::Value* ElseStatement::codegen() {
  return nullptr;
}

ElseIfStatement::ElseIfStatement(){
  printf(">>>>>ElseIfStatement\n");
}

llvm::Value* ElseIfStatement::codegen() {
  return nullptr;
}

ElseIfStatements::ElseIfStatements(){
  printf(">>>>>ElseIfStatements\n");
}
llvm::Value* ElseIfStatements::codegen() {
  return nullptr;
}

IfStatement::IfStatement(){
  printf(">>>>>IfStatement\n");
}

llvm::Value* IfStatement::codegen() {
  return nullptr;
}

ForStatement::ForStatement(){
  printf(">>>>>ForStatement\n");
}

llvm::Value* ForStatement::codegen() {
  return nullptr;
}

WhileStatement::WhileStatement(){
  printf(">>>>>WhileStatement\n");
}

llvm::Value* WhileStatement::codegen() {
  return nullptr;
}

DoWhileStatement::DoWhileStatement(){
  printf(">>>>>DoWhileStatement\n");
}

llvm::Value* DoWhileStatement::codegen() {
  return nullptr;
}

Statement::Statement(){
  printf(">>>>>Statement\n");
}

llvm::Value* Statement::codegen() {
  return nullptr;
}

StatementComponent::StatementComponent(){
  printf(">>>>>StatementComponent\n");
}
llvm::Value* StatementComponent::codegen() {
  return nullptr;
}

StatementComponents::StatementComponents(){
  printf(">>>>>StatementComponents\n");
}

llvm::Value* StatementComponents::codegen() {
  return nullptr;
}

