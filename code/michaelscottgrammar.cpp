#include "michaelscottgrammar.h"

std::map<std::string, llvm::AllocaInst*> LLVMNamedValues;
std::map<std::string, std::map<std::string, llvm::AllocaInst*>> LLVMVariables;
//nao sei qual function
std::map<std::string, std::unique_ptr<llvm::Function>> LLVMFunctions;
llvm::Type *current_type;
llvm::Type *function_type;

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


void MichaelScottNode::error()
{
  printf("**Error [line %d]: unknown\n", this->localLineNumber);
  exit(1);
}

void MichaelScottNode::error(const char* error_warning)
{
  printf("**Error [line %d]: %s\n", this->localLineNumber, error_warning);
  exit(1);
}

void MichaelScottNode::error(const char* error_warning, std::string error_text)
{
  printf("**Error [line %d]: %s\n\t\t%s\n", this->localLineNumber, error_warning, error_text.c_str());
  exit(1);
}

/*****************************************************************************/
/******************* Methods related to program ******************************/

MichaelScottNode::MichaelScottNode(){
  this->localLineNumber = globalLineNumber;
}

MichaelScott::MichaelScott(){
  printf(">>>>>MichaelScott\n");
}

llvm::Value* MichaelScott::codegen() {
  InitializeModule();
  std::cout << std::endl;
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
  std::string s = "";
  if(this->negative) s = "MINUS ";
  return s+name;
}

llvm::Value* Variable::codegen() {
  std::cout << "VARIABLE" << std::endl;
  std::cout << LLVMNamedValues[this->name]->getAllocatedType()->getTypeID() << std::endl;
  std::cout << current_type->getTypeID()   << std::endl;
  llvm::Value* temp = NULL;
  if(LLVMNamedValues.find(this->name) == LLVMNamedValues.end()) 
      error("Variable not declared.", this->name);
  if(current_type and current_type != LLVMNamedValues[this->name]->getAllocatedType()){
    temp = LLVMBuilder->CreateLoad(LLVMNamedValues[this->name], "casttemp");
    if(current_type->isFP128Ty())
      temp = createCastLLVM(temp, llvm::Type::getFP128Ty(*LLVMContext));
    else if(current_type->isDoubleTy())
      temp = createCastLLVM(temp, llvm::Type::getDoubleTy(*LLVMContext));
    else if(current_type->isFloatingPointTy())
      temp = createCastLLVM(temp, llvm::Type::getFloatTy(*LLVMContext));
    else if(current_type->isIntegerTy(128))
      temp = createCastLLVM(temp, llvm::Type::getInt128Ty(*LLVMContext));
    else if(current_type->isIntegerTy(64))
      temp = createCastLLVM(temp, llvm::Type::getInt64Ty(*LLVMContext));
    else if(current_type->isIntegerTy())
      temp = createCastLLVM(temp, llvm::Type::getInt32Ty(*LLVMContext));
    else error("Uncastable type", this->name);
    if(!this->negative) return temp;
  }
  else if(current_type and current_type == LLVMNamedValues[this->name]->getAllocatedType()){
    if(current_type->isIntegerTy()){
      if(!current_type->isIntegerTy(128) and !current_type->isIntegerTy(64) and !current_type->isIntegerTy(32)){
        temp = LLVMBuilder->CreateLoad(LLVMNamedValues[this->name], "casttemp");
        temp = createCastLLVM(temp, llvm::Type::getInt32Ty(*LLVMContext));
        if(!this->negative) return temp;
      }
    }
    else if(current_type->isFloatingPointTy()){
      if(!current_type->isFP128Ty() and !current_type->isFloatTy() and !current_type->isDoubleTy()){
        temp = LLVMBuilder->CreateLoad(LLVMNamedValues[this->name], "casttemp");
        temp = createCastLLVM(temp, llvm::Type::getFloatTy(*LLVMContext));
        if(!this->negative) return temp;
      }
    }
    else error("No extend for this type", this->name);
  }
  if(this->negative){
    if(!temp) temp = LLVMBuilder->CreateLoad(LLVMNamedValues[this->name], "negtemp");
    temp = negativeExpression(current_type, temp);
    if(temp) return temp;
    else error("Impossible to negative this variable", this->name);
  }

  temp = LLVMBuilder->CreateLoad(LLVMNamedValues[this->name]);
  return temp;
}

llvm::AllocaInst* Variable::getAllocated(){
  if(LLVMNamedValues.find(this->name) == LLVMNamedValues.end()) 
      error("Variable not declared.", this->name);
  return LLVMNamedValues[this->name];
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

    if(LLVMNamedValues.find(var->name) != LLVMNamedValues.end()) 
        error("Variable already declared.", var->name);

    llvm::AllocaInst *alloca = createEntryBlockAlloca(F, var->name, T);
    LLVMBuilder->CreateStore(init, alloca);
    LLVMNamedValues[var->name] = alloca;
  }

  return nullptr;
}

Number::Number(){
  printf(">>>>>Number\n");
}

Number::Number(double val):val(val){}

std::string Number::print(){
  std::string s = "";
  if(this->negative) s = "MINUS ";
  return s+std::to_string(val);
}

llvm::Value* Number::codegen() {
  std::cout << "NUMBER" << std::endl;
  double value = this->val;
  if(this->negative) value = (-1)*value;
  if(current_type->isFP128Ty())
    return llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(value));
  else if(current_type->isDoubleTy())
    return llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(value));
  else if(current_type->isFloatingPointTy())
    return llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(value));
  else if(current_type->isIntegerTy(128))
    return llvm::ConstantInt::get(*LLVMContext, llvm::APInt(128, value));
  else if(current_type->isIntegerTy(64))
    return llvm::ConstantInt::get(*LLVMContext, llvm::APInt(64, value));
  else if(current_type->isIntegerTy())
    return llvm::ConstantInt::get(*LLVMContext, llvm::APInt(32, value));
  return nullptr;
}

/*****************************************************************************/
/******************* Methods related to expressions **************************/

MichaelScottExpressionNode::MichaelScottExpressionNode() : negative(false){};


Assignment::Assignment(){
  e = NULL;
  f = NULL;
  printf(">>>>>Assignment\n");
}

llvm::Value* Assignment::codegen() {
  std::cout<< "ASSIGNMENT" << std::endl;
  current_type = NULL;
  llvm::AllocaInst *variable = this->v->getAllocated();
  current_type = variable->getAllocatedType();
  std::cout << current_type->getTypeID() << std::endl;
  llvm::Value *assignment_value;
  if(this->e) assignment_value = this->e->codegen();
  else if(this->f) assignment_value = this->f->codegen();
  std::cout << assignment_value->getType()->getTypeID() << " " <<  current_type->getTypeID() << std::endl;
  assignment_value = createCastLLVM(assignment_value, current_type);
  printf("___________________\n");
  LLVMBuilder->CreateStore(assignment_value, variable);
  return nullptr;
}

Assignments::Assignments(){
  printf(">>>>>Assignments\n");
}

llvm::Value* Assignments::codegen() {
  std::cout<< "ASSIGNMENTS" << std::endl;
  return nullptr;
}

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
  std::string s = "";
  std::string f = "";
  if(this->negative) {s = "(MINUS "; f=")";}
  return s+">"+lhs+"< "+op+" >"+rhs+"<"+f;
}

llvm::Value* Expression::codegen() {
  std::cout << "EXPRESSION" << std::endl;

  llvm::Value *L = this->LHS->codegen();
  llvm::Value *R = this->RHS->codegen();
  if (!L || !R)
    return nullptr;
  llvm::Value *returned_expression;
  if(this->op == "+"){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFAdd(L, R, "addtmp");
    else if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateAdd(L, R, "addtmp");
    else return nullptr;
    if(this->negative) returned_expression = negativeExpression(current_type, returned_expression);
    return returned_expression;
  }
  else if(this->op == "-"){
    std::cout<< current_type->getTypeID() << std::endl;
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFSub(L, R, "subtmp");
    else if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateSub(L, R, "subtmp");
    else return nullptr;
    if(this->negative) returned_expression = negativeExpression(current_type, returned_expression);
    return returned_expression;
  }
  else if(this->op == "*"){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFMul(L, R, "multmp");
    else if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateMul(L, R, "multmp");
    else return nullptr;
    if(this->negative) returned_expression = negativeExpression(current_type, returned_expression);
    return returned_expression;
  }
  else if(this->op == "/"){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFDiv(L, R, "divtmp");
    else if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateSDiv(L, R, "divtmp");
    else return nullptr;
    if(this->negative) returned_expression = negativeExpression(current_type, returned_expression);
    return returned_expression;
  }
  else if(this->op == "%"){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFRem(L, R, "remtmp");
    else if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateSRem(L, R, "remtmp");
    else return nullptr;
    if(this->negative) returned_expression = negativeExpression(current_type, returned_expression);
    return returned_expression;
  }
  else if(this->op == "<"){
    L = LLVMBuilder->CreateFCmpULT(L, R, "cmptmp");
    // Convert bool 0/1 to double 0.0 or 1.0
    return LLVMBuilder->CreateUIToFP(L, llvm::Type::getDoubleTy(*LLVMContext), "booltmp");
  }
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

int LogicExpression::getRHSType(){ 
  return 1; 
};

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
  current_type = function_type;
  if(function_type == llvm::Type::getVoidTy(*LLVMContext)){
    if(this->v) error("You are trying to return a paper in a CREED function");
    return LLVMBuilder->CreateRetVoid();
  }
  else if(this->v){
    //todo: nao ta fazendo cast
    llvm::Value* temp = LLVMBuilder->CreateLoad(this->v->getAllocated(), "return");
    return LLVMBuilder->CreateRet(temp);
  }
  error("You have not defined a value to return");
  return nullptr;
}

FunctionComponent::FunctionComponent(){
  fc = NULL;
  fr = NULL;
  vd = NULL;
  s = NULL;
  a = NULL;
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

  std::vector<llvm::Type *> ptypes;
  std::vector<std::string> pnames;

  for (auto i = p->iterator(); i->hasNext();){
    Parameter *par = i->next(); 
    std::string type = par->type;
    std::string name = par->name;
    if(std::find(pnames.begin(), pnames.end(), name) != pnames.end())
      error("More than one parameter with the same name in the function",
            this->name + " >> " + name);
    pnames.push_back(name);
    ptypes.push_back(getTypeLLVM(type));
  }

  llvm::FunctionType *FT = llvm::FunctionType::get(getTypeLLVM(this->return_type), 
                                                    ptypes, false);

  llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, 
                                              this->name, LLVMModule.get());
  function_type = FT->getReturnType();
  
  // Set names for all arguments.
  unsigned idx = 0;
  for (auto &arg : F->args()){
    arg.setName(pnames[idx++]);
  }    
  

  llvm::BasicBlock *BBentry = llvm::BasicBlock::Create(*LLVMContext, "entry", F);
  LLVMBuilder->SetInsertPoint(BBentry);
  /*
  int num_func=1;
  std::string name_func;
  while(true){
    name_func = std::string(this->name)+"_"+std::to_string(num_func);
    if (LLVMVariables.find(name_func) == LLVMVariables.end()) {
      break;
    }
    num_func++;
  }
  */
  LLVMNamedValues.clear();
  for (auto &Arg : F->args()) {
    std::string arg_name = std::string(Arg.getName());
    // Create an alloca for this variable.
    llvm::AllocaInst *alloca = createEntryBlockAlloca(F, arg_name+"_", Arg.getType());

    // Store the initial value into the alloca.
    LLVMBuilder->CreateStore(&Arg, alloca);

    // Add arguments to variable symbol table.
    LLVMNamedValues[arg_name] = alloca;
  }
  
/*
  for (auto it = LLVMNamedValues.begin(); it != LLVMNamedValues.end(); it++)
  {
      std::cout << it->first    // string (key)
            << ':'
            << it->second
            << std::endl; 
  }
*/

  this->body->codegen();


  // Validate the generated code, checking for consistency.
  llvm::verifyFunction(*F);

  // Run the optimizer on the function.
  //LLVMFPM->run(*F);

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
  std::cout<< "ELSE IF STATEMENT" << std::endl;
  return nullptr;
}

ElseIfStatements::ElseIfStatements(){
  printf(">>>>>ElseIfStatements\n");
}
llvm::Value* ElseIfStatements::codegen() {
  return nullptr;
}

IfStatement::IfStatement(){
  this->c = NULL;
  this->e = NULL;
  this->ei = NULL;
  printf(">>>>>IfStatement\n");
}

llvm::Value* IfStatement::codegen() {
  std::cout<< "IF STATEMENT" << std::endl;
  
  llvm::Value *logic = this->l->codegen();
  logic = llvm::ConstantInt::get(*LLVMContext, llvm::APInt(1, 0));

  llvm::Value *CondV = LLVMBuilder->CreateFCmpONE(logic, llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(0.0)), "ifcond");
  
  llvm::Function *F = LLVMBuilder->GetInsertBlock()->getParent();

  llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*LLVMContext, "then", F);
  llvm::BasicBlock *EndBB = llvm::BasicBlock::Create(*LLVMContext, "end");
  llvm::BasicBlock *ElseBB, *ElifCondBB, *ElifBB, *NextBB;

  if(this->ei){
    ElifCondBB = llvm::BasicBlock::Create(*LLVMContext, "elifcond");
    LLVMBuilder->CreateCondBr(CondV, ThenBB, ElifCondBB);
    if(this->e) ElseBB = llvm::BasicBlock::Create(*LLVMContext, "else");
  }
  else if(this->e){
    ElseBB = llvm::BasicBlock::Create(*LLVMContext, "else");
    LLVMBuilder->CreateCondBr(CondV, ThenBB, ElseBB);
  }
  else
    LLVMBuilder->CreateCondBr(CondV, ThenBB, EndBB);

  LLVMBuilder->SetInsertPoint(ThenBB);
  this->c->codegen();
  LLVMBuilder->CreateBr(EndBB);

  if(this->ei){
    NextBB = NULL;
    for(auto i = ei->iterator(); i->hasNext();){
      ElseIfStatement *elif = i->next(); 

      F->getBasicBlockList().push_back(ElifCondBB);
      LLVMBuilder->SetInsertPoint(ElifCondBB);

      llvm::Value *eliflogic = elif->l->codegen();
      eliflogic = llvm::ConstantInt::get(*LLVMContext, llvm::APInt(1, 0));
      CondV = LLVMBuilder->CreateFCmpONE(eliflogic, llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(0.0)), "ifcond2");
      
      ElifBB = llvm::BasicBlock::Create(*LLVMContext, "elif");
      if(i->hasNext()){
        ElifCondBB = llvm::BasicBlock::Create(*LLVMContext, "elifcond");
      }
      else if(this->e) ElifCondBB = ElseBB;
      else ElifCondBB = EndBB;
      LLVMBuilder->CreateCondBr(CondV, ElifBB, ElifCondBB);

      F->getBasicBlockList().push_back(ElifBB);
      LLVMBuilder->SetInsertPoint(ElifBB);
      elif->c->codegen();
      LLVMBuilder->CreateBr(EndBB);
      //if(i->hasNext()) ElifBB = NextBB;
    }
  }

  if(this->e){
    F->getBasicBlockList().push_back(ElseBB);
    LLVMBuilder->SetInsertPoint(ElseBB);
    this->e->c->codegen();
    LLVMBuilder->CreateBr(EndBB);
  }
  
  F->getBasicBlockList().push_back(EndBB);
  LLVMBuilder->SetInsertPoint(EndBB);
  //llvm::PHINode *PN = LLVMBuilder->CreatePHI(llvm::Type::getDoubleTy(*LLVMContext), 2, "iftmp");

  //PN->addIncoming(ThenV, ThenBB);
  //PN->addIncoming(ElseV, ElseBB);
  return nullptr; //PN;
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
  i = NULL;
  f = NULL;
  w = NULL;
  d = NULL;
  printf(">>>>>Statement\n");
}

llvm::Value* Statement::codegen() {
  std::cout<< "STATEMENT" << std::endl;
  if(i) i->codegen();
  else if(f) f->codegen();
  else if(w) w->codegen();
  else if(d) d->codegen();
  return nullptr;
}

StatementComponent::StatementComponent(){
  fc = NULL;
  fr = NULL;
  vd = NULL;
  s = NULL;
  a = NULL;
  printf(">>>>>StatementComponent\n");
}
llvm::Value* StatementComponent::codegen() {
  std::cout<< "STATEMENT COMPONENT" << std::endl;
  if(fc) fc->codegen();
  else if(fr) fr->codegen();
  else if(vd) vd->codegen();
  else if(s) s->codegen();
  else if(a) a->codegen();
  return nullptr;
}

StatementComponents::StatementComponents(){
  printf(">>>>>StatementComponents\n");
}

llvm::Value* StatementComponents::codegen() {
  for (auto i = this->iterator(); i->hasNext();)
    i->next()->codegen();
  return nullptr;
}

