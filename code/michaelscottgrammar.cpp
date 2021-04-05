#include "michaelscottgrammar.h"

std::map<std::string, llvm::AllocaInst*> LLVMNamedValues;
std::map<std::string, llvm::GlobalVariable*> LLVMGlobalValues;
std::map<std::string, std::map<std::string, llvm::AllocaInst*>> LLVMVariables;
//nao sei qual function
std::map<std::string, std::unique_ptr<llvm::Function>> LLVMFunctions;
llvm::Type *current_type;
llvm::Type *function_type;
bool return_instr;

llvm::Function *getFunction(std::string name) {
  if(auto *F = LLVMModule->getFunction(name))
    return F;
  return nullptr;
}

bool variableExists(std::string name){
  bool exists = false;
  if(LLVMNamedValues.find(name) != LLVMNamedValues.end())
    exists = true;
  if(LLVMGlobalValues.find(name) != LLVMGlobalValues.end())
    exists = true;
  return exists;
}

bool variableIsGlobal(std::string name){
  bool is_global = false;
  if(LLVMGlobalValues.find(name) != LLVMGlobalValues.end())
    is_global = true;
  return is_global;
}

bool variableIsLocal(std::string name){
  bool is_local = false;
  if(LLVMNamedValues.find(name) != LLVMNamedValues.end())
    is_local = true;
  return is_local;
}

llvm::Type* getVariableType(std::string name){
  bool exists = false;
  if(variableExists(name)){
    if(LLVMNamedValues.find(name) != LLVMNamedValues.end())
      return LLVMNamedValues[name]->getAllocatedType();
    if(LLVMGlobalValues.find(name) != LLVMGlobalValues.end())
      return LLVMGlobalValues[name]->getType()->getElementType();
  }
  printf("**Error This variable does not exist: %s\n", name.c_str());
  exit(1);
  return nullptr;
}

llvm::AllocaInst* getLocalVariable(std::string name){
  bool exists = false;
  if(variableExists(name)){
    if(LLVMNamedValues.find(name) != LLVMNamedValues.end())
      return LLVMNamedValues[name]; 
  }
  printf("**Error This variable does not exist: %s\n", name.c_str());
  exit(1);
  return nullptr;
}

llvm::GlobalVariable* getGlobalVariable(std::string name){
  bool exists = false;
  if(variableExists(name)){
    if(LLVMGlobalValues.find(name) != LLVMGlobalValues.end())
      return LLVMGlobalValues[name];  
  }
  printf("**Error This variable does not exist: %s\n", name.c_str());
  exit(1);
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
  return nullptr;
}

GlobalVariableDeclaration::GlobalVariableDeclaration(){
  printf(">>>>>GlobalVariableDeclaration\n");
}

llvm::Value* GlobalVariableDeclaration::codegen() {
  std::cout<< "GLOBAL VARIABLE DECLARATION" << std::endl;
  llvm::Type *T = getTypeLLVM(this->vd->type);
  llvm::Constant *init = LLVMValuesInits[this->vd->type];
  for (auto i = this->vd->v->iterator(); i->hasNext();){
    Variable *var = (Variable *) i->next();
    if(LLVMGlobalValues.find(var->name) != LLVMGlobalValues.end()) 
        error("Global variable already declared.", var->name);
    llvm::GlobalVariable* globalvar = new llvm::GlobalVariable(*LLVMModule, T, false,
                                  llvm::GlobalValue::CommonLinkage, init, "global");
    //globalvar->setAlignment(llvm::MaybeAlign(4));
    LLVMGlobalValues[var->name] = globalvar;
  }
  return nullptr;
}

Component::Component(){
  i = NULL;
  gvd = NULL;
  f = NULL;
  main = NULL;
  printf(">>>>>Component\n");
}

llvm::Value* Component::codegen() {
  std::cout<< "COMPONENT" << std::endl;
  if(i) i->codegen();
  else if(gvd) gvd->codegen();
  else if(f) f->codegen();
  else if(main) main->codegen();
  return nullptr;
}

Components::Components(){
  printf(">>>>>Components\n");
}
llvm::Value* Components::codegen() {
  //for(it = this->iterator(); this->iterator().hasNext(); this)
  for (IteratorPtr i = this->iterator(); i->hasNext();)
    i->next()->codegen();
  return nullptr;
}

/******************* Methods related to variables ****************************/
Variable::Variable(){
  this->array = false;
  printf(">>>>>Variable\n");
}

Variable::Variable(std::string type):type(move(type)){
  this->array = false;
  printf(">>>>>Variable\n");
}

Variable::Variable(std::string type, std::string name):
            type(move(type)), name(move(name)){
  this->array = false;
  printf(">>>>>Variable\n");
}

std::string Variable::print(){
  std::string s = "";
  if(this->negative) s = "MINUS ";
  return s+name;
}

llvm::Value* Variable::codegen() {
  std::cout << "VARIABLE" << std::endl;
  //std::cout << LLVMNamedValues[this->name]->getAllocatedType()->getTypeID() << std::endl;
  //std::cout << current_type->getTypeID()   << std::endl;
  llvm::Value* temp = NULL;
  if(!variableExists(this->name)) 
    error("Variable not declared.", this->name);
  if(current_type and current_type != getVariableType(this->name)){
    if(variableIsLocal(this->name))
      temp = LLVMBuilder->CreateLoad(getLocalVariable(this->name), "casttemp");
    else if(variableIsGlobal(this->name))
      temp = LLVMBuilder->CreateLoad(getGlobalVariable(this->name), "casttemp");
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
  else if(current_type and current_type == getVariableType(this->name)){
    if(current_type->isIntegerTy()){
      if(!current_type->isIntegerTy(128) and !current_type->isIntegerTy(64) and !current_type->isIntegerTy(32)){
        if(variableIsLocal(this->name))
          temp = LLVMBuilder->CreateLoad(getLocalVariable(this->name), "casttemp");
        else if(variableIsGlobal(this->name))
          temp = LLVMBuilder->CreateLoad(getGlobalVariable(this->name), "casttemp");
        temp = createCastLLVM(temp, llvm::Type::getInt32Ty(*LLVMContext));
        if(!this->negative) return temp;
      }
    }
    else if(current_type->isFloatingPointTy()){
      if(!current_type->isFP128Ty() and !current_type->isFloatTy() and !current_type->isDoubleTy()){
        if(variableIsLocal(this->name))
          temp = LLVMBuilder->CreateLoad(getLocalVariable(this->name), "casttemp");
        else if(variableIsGlobal(this->name))
          temp = LLVMBuilder->CreateLoad(getGlobalVariable(this->name), "casttemp");
        temp = createCastLLVM(temp, llvm::Type::getFloatTy(*LLVMContext));
        if(!this->negative) return temp;
      }
    }
    else error("No extend for this type", this->name);
  }
  if(this->negative){
    if(!temp){
      if(variableIsLocal(this->name))
        temp = LLVMBuilder->CreateLoad(getLocalVariable(this->name), "negtemp");
      else if(variableIsGlobal(this->name))
        temp = LLVMBuilder->CreateLoad(getGlobalVariable(this->name), "negtemp");
    }
    temp = negativeExpression(current_type, temp);
    if(temp) return temp;
    else error("Impossible to negative this variable", this->name);
  }
  if(variableIsLocal(this->name))
    temp = LLVMBuilder->CreateLoad(getLocalVariable(this->name));
  else if(variableIsGlobal(this->name))
    temp = LLVMBuilder->CreateLoad(getGlobalVariable(this->name));
  return temp;
}

llvm::Type* Variable::getTypeOfExpression(){
  if(!variableExists(this->name))
    error("Variable not declared.", this->name);
  return getVariableType(this->name);
}

Variables::Variables(){
  printf(">>>>>Variables\n");
}

llvm::Value* Variables::codegen() {
  std::cout << "VARIABLESSSS" << std::endl;
  return nullptr;
}

ArrayDeclaration::ArrayDeclaration(){
  printf(">>>>>ArrayDeclaration\n");
}

llvm::Value* ArrayDeclaration::codegen() {
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

    if(variableExists(var->name))
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
    return llvm::ConstantFP::get(llvm::Type::getFP128Ty(*LLVMContext), value);
  else if(current_type->isDoubleTy())
    return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*LLVMContext), value);
  else if(current_type->isFloatTy())
    return llvm::ConstantFP::get(llvm::Type::getFloatTy(*LLVMContext), value);
  else if(current_type->isHalfTy())
    return llvm::ConstantFP::get(llvm::Type::getHalfTy(*LLVMContext), value);
  else if(current_type->isFloatingPointTy())
    return llvm::ConstantFP::get(llvm::Type::getFloatTy(*LLVMContext), value);

  else if(current_type->isIntegerTy(128))
    return llvm::ConstantInt::get(llvm::Type::getInt128Ty(*LLVMContext), value, true);
  else if(current_type->isIntegerTy(64))
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*LLVMContext), value, true);
  else if(current_type->isIntegerTy(32))
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*LLVMContext), value, true);
  else if(current_type->isIntegerTy(16))
    return llvm::ConstantInt::get(llvm::Type::getInt16Ty(*LLVMContext), value, true);
  else if(current_type->isIntegerTy(8))
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*LLVMContext), value, true);
  else if(current_type->isIntegerTy(1))
    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*LLVMContext), value);
  else if(current_type->isIntegerTy())
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*LLVMContext), value, true);
  return nullptr;
}

llvm::Type* Number::getTypeOfExpression(){
  if(floor(this->val) == this->val)
    return llvm::Type::getFloatTy(*LLVMContext);
  else
    return llvm::Type::getInt32Ty(*LLVMContext);
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

  if(variableIsLocal(this->v->name)){
    llvm::AllocaInst *variable = getLocalVariable(this->v->name);
    current_type = variable->getAllocatedType();
    llvm::Value *assignment_value;
    if(this->e) assignment_value = this->e->codegen();
    else if(this->f) assignment_value = this->f->codegen();
    //std::cout << assignment_value->getType()->getTypeID() << " " <<  current_type->getTypeID() << std::endl;
    assignment_value = createCastLLVM(assignment_value, current_type);
    LLVMBuilder->CreateStore(assignment_value, variable);
  }
  else if(variableIsGlobal(this->v->name)){
    llvm::GlobalVariable *globalvariable = getGlobalVariable(this->v->name);
    current_type = globalvariable->getType()->getElementType();
    llvm::Value *assignment_value;
    if(this->e) assignment_value = this->e->codegen();
    else if(this->f) assignment_value = this->f->codegen();
    //std::cout << assignment_value->getType()->getTypeID() << " " <<  current_type->getTypeID() << std::endl;
    assignment_value = createCastLLVM(assignment_value, current_type);
    LLVMBuilder->CreateStore(assignment_value, globalvariable);
  }
  return nullptr;
}

Assignments::Assignments(){
  printf(">>>>>Assignments\n");
}

llvm::Value* Assignments::codegen() {
  std::cout<< "ASSIGNMENTS" << std::endl;
  //for(it = this->iterator(); this->iterator().hasNext(); this)
  for (IteratorPtr i = this->iterator(); i->hasNext();)
    i->next()->codegen();
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

llvm::Type* Expression::getTypeOfExpression(){
  return this->LHS->getTypeOfExpression();
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
  std::cout << "LOGIC EXPRESSION" << std::endl;
  current_type = this->LHS->getTypeOfExpression();
  
  llvm::Value *L = this->LHS->codegen();
  llvm::Value *R = this->RHS->codegen();
  printf("%p %p %s", L, R, this->op.c_str());
  if (!L || !R)
    return nullptr;

  llvm::Value *returned_expression;
  if(this->op == "<"){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFCmpULT(L, R, "lesstmp");
    if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateICmpSLT(L, R, "lesstmp");
    else return nullptr;
    if(this->notLHS) returned_expression = LLVMBuilder->CreateNot(returned_expression, "nottmp");
    return returned_expression;
    // Convert bool 0/1 to double 0.0 or 1.0
    //return LLVMBuilder->CreateUIToFP(L, llvm::Type::getDoubleTy(*LLVMContext),"booltmp");
  }
  else if(this->op == "<="){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFCmpULE(L, R, "lessequaltmp");
    if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateICmpSLE(L, R, "lessequaltmp");
    else return nullptr;
    if(this->notLHS) returned_expression = LLVMBuilder->CreateNot(returned_expression, "nottmp");
    return returned_expression;
  }
  else if(this->op == ">"){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFCmpUGT(L, R, "greattmp");
    if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateICmpSGT(L, R, "greattmp");
    else return nullptr;
    if(this->notLHS) returned_expression = LLVMBuilder->CreateNot(returned_expression, "nottmp");
    return returned_expression;
  }
  else if(this->op == ">="){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFCmpUGE(L, R, "greatequaltmp");
    if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateICmpSGE(L, R, "greatequaltmp");
    else return nullptr;
    if(this->notLHS) returned_expression = LLVMBuilder->CreateNot(returned_expression, "nottmp");
    return returned_expression;
  }
  else if(this->op == "=="){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFCmpUEQ(L, R, "equaltmp");
    if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateICmpEQ(L, R, "equaltmp");
    else return nullptr;
    if(this->notLHS) returned_expression = LLVMBuilder->CreateNot(returned_expression, "nottmp");
    return returned_expression;
  }
  else if(this->op == "=!"){
    if(current_type->isFloatingPointTy())
      returned_expression = LLVMBuilder->CreateFCmpUNE(L, R, "notequaltmp");
    if(current_type->isIntegerTy())
      returned_expression = LLVMBuilder->CreateICmpNE(L, R, "notequaltmp");
    else return nullptr;
    if(this->notLHS) returned_expression = LLVMBuilder->CreateNot(returned_expression, "nottmp");
    return returned_expression;
  }
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

int Logic::getRHSType(){
  return 2;
};

llvm::Value* Logic::codegen() {
  std::cout << "LOGIC" << std::endl;
  llvm::Value *L = this->LHS->codegen();
  llvm::Value *R = NULL;
  if(this->RHS) R = this->RHS->codegen();
  else return L;
  llvm::Value *returned_expression;

  if(this->op == "snap")
    returned_expression = LLVMBuilder->CreateAnd(L, R, "andtmp");
  else if(this->op == "snip")
    returned_expression = LLVMBuilder->CreateOr(L, R, "ortmp");
  else if(this->op == "no god no")
    returned_expression = LLVMBuilder->CreateNot(R, "nottmp");
  return returned_expression;
}

/*****************************************************************************/
/******************* Methods related to functions **************************/

Argument::Argument(){
  printf(">>>>>Argument\n");
  num = NULL;
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
  this->args = NULL;
  printf(">>>>>FunctionCall\n");
}

llvm::Value* FunctionCall::codegen() {
  std::cout<< "FUNCTION CALL" << std::endl;
  llvm::Function *function = getFunction(this->name);
  llvm::FunctionCallee functionprot;
  if(!function){
    printf("Unknown function reference, making a prototype for: %s\n", this->name.c_str());
    std::vector<llvm::Type *> ptypes;
    for (auto i = this->args->iterator(); i->hasNext();){
      Argument *arg = i->next();
      llvm::Type *type = getVariableType(arg->name);
      ptypes.push_back(type);
    }
    llvm::Type *rtype = current_type;
    llvm::FunctionType *ftype = llvm::FunctionType::get(rtype, ptypes, false);
    functionprot = LLVMModule->getOrInsertFunction(this->name.c_str(), ftype);
  }
  else if(function->arg_size() != this->args->size()){
    printf("Incorrect amount arguments passed for function, making a prototype for: %s", this->name.c_str());
    std::vector<llvm::Type *> ptypes;
    for (auto i = this->args->iterator(); i->hasNext();){
      Argument *arg = i->next();
      llvm::Type *type = getVariableType(arg->name);
      ptypes.push_back(type);
    }
    llvm::Type *rtype = current_type;
    llvm::FunctionType *ftype = llvm::FunctionType::get(rtype, ptypes, false);
    functionprot = LLVMModule->getOrInsertFunction(this->name.c_str(), ftype);
  }
  std::vector<llvm::Value *> arguments;
  for(auto i = this->args->iterator(); i->hasNext();){
    Argument *arg = i->next();
    if(arg->num)
      arguments.push_back(arg->num->codegen());
    else{
      if(variableIsLocal(arg->name)){
        // TODO: pointer
        llvm::AllocaInst* variable = getLocalVariable(arg->name);
        llvm::Value* loaded_variable = LLVMBuilder->CreateLoad(variable, "arg");
        arguments.push_back(loaded_variable);
      }
      else if(variableIsGlobal(arg->name)){
        error("You should not use global variables as parameter", arg->name);
      }
    }
  }
  if(function)
    return LLVMBuilder->CreateCall(function, arguments, "calltmp");
  else{
    return LLVMBuilder->CreateCall(functionprot, arguments, "calltmp");
  }
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
  return_instr = true;
  current_type = function_type;
  if(function_type == llvm::Type::getVoidTy(*LLVMContext))
    error("You are trying to return a paper in a CREED function");
  else if(this->v){
    //todo: nao ta fazendo cast
    //llvm::Value* temp = LLVMBuilder->CreateLoad(this->v->getAllocated(), "return");
    llvm::Value* temp = this->v->codegen();
    return LLVMBuilder->CreateRet(temp);
  }
  error("You have not defined a value to return");
  return nullptr;
}

FunctionComponent::FunctionComponent(){
  fc = NULL;
  fr = NULL;
  fp = NULL;
  fs = NULL;
  vd = NULL;
  s = NULL;
  a = NULL;
  printf(">>>>>FunctionComponent\n");
}

llvm::Value* FunctionComponent::codegen() {
  std::cout<< "FUNCTION COMPONENT" << std::endl;
  if(fc) fc->codegen();
  else if(fr) fr->codegen();
  else if(fp) fp->codegen();
  else if(fs) fs->codegen();
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

  if(getFunction(this->name))
    error("This function already exists.", this->name);

  for (auto i = this->p->iterator(); i->hasNext();){
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
  
  return_instr = false;
  this->body->codegen();
  if(function_type == llvm::Type::getVoidTy(*LLVMContext))
    LLVMBuilder->CreateRetVoid();
  else if(!return_instr)
    error("No return from function", this->name);
  
  // Validate the generated code, checking for consistency.
  llvm::verifyFunction(*F);

  // Run the optimizer on the function.
  //LLVMFPM->run(*F);

  F->print(llvm::errs());

  return F;
}


/*****************************************************************************/
/******************* Methods related to main function ************************/
bool MainFunction::created = false;

MainFunction::MainFunction(){
  if(this->created)
    error("There is more than one Scranton function.");
  this->created = true;
  printf(">>>>>MainFunction\n");
}

llvm::Value* MainFunction::codegen() {
  std::cout<< "MAIN FUNCTION" << std::endl;
  std::vector<llvm::Type *> ptypes;
  std::vector<std::string> pnames;

  for (auto i = this->p->iterator(); i->hasNext();){
    Parameter *par = i->next(); 
    std::string type = par->type;
    std::string name = par->name;
    if(std::find(pnames.begin(), pnames.end(), name) != pnames.end())
      error("More than one parameter with the same name in the function",
            "Scranton >> " + name);
    pnames.push_back(name);
    ptypes.push_back(getTypeLLVM(type));
  }

  llvm::FunctionType *main_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*LLVMContext), 
                                                    //ptypes,
                                                    false);

  llvm::Function *main = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, 
                                                "main", LLVMModule.get());
  function_type = main_type->getReturnType();
  
  // Set names for all arguments.
  unsigned idx = 0;
  for (auto &arg : main->args()){
    arg.setName(pnames[idx++]);
  }
  

  llvm::BasicBlock *BBentry = llvm::BasicBlock::Create(*LLVMContext, "entry", main);
  LLVMBuilder->SetInsertPoint(BBentry);
  // TODO: argc e argv
  LLVMNamedValues.clear();
  for (auto &Arg : main->args()) {
    std::string arg_name = std::string(Arg.getName());
    // Create an alloca for this variable.
    llvm::AllocaInst *alloca = createEntryBlockAlloca(main, arg_name+"_", Arg.getType());

    // Store the initial value into the alloca.
    LLVMBuilder->CreateStore(&Arg, alloca);

    // Add arguments to variable symbol table.
    LLVMNamedValues[arg_name] = alloca;
  }
  
  this->body->codegen();

  LLVMBuilder->CreateRet(llvm::ConstantInt::get(*LLVMContext, llvm::APInt(32, 0)));

  // Validate the generated code, checking for consistency.
  llvm::verifyFunction(*main);

  // Run the optimizer on the function.
  //LLVMFPM->run(*F);

  main->print(llvm::errs());
  return main;
}


/*****************************************************************************/
/******************* Methods related to print function ***********************/

PrintFunctionCall::PrintFunctionCall(){
  this->args = NULL;
  printf(">>>>>PrintFunctionCall\n");
}

llvm::Value* PrintFunctionCall::codegen() {
  std::cout<< "PRINT FUNCTION" << std::endl;
  llvm::Type *intType = llvm::Type::getInt32Ty(*LLVMContext);
  std::vector<llvm::Type *> printfArgsTypes({llvm::Type::getInt8PtrTy(*LLVMContext)});
  llvm::FunctionType *printfType = llvm::FunctionType::get(intType, printfArgsTypes, true);
  llvm::FunctionCallee printfFunc = LLVMModule->getOrInsertFunction("printf", printfType);
  
  std::string print_string = "";
  std::vector<llvm::Value *> arguments;
  for(auto i = this->args->iterator(); i->hasNext();){
    Argument *arg = i->next();
    // Get the arguments
    if(!print_string.empty()) print_string += " ";
    if(!arg->str.empty())
      print_string.append(arg->str);
    else{
      if(!variableExists(arg->name))
        error("Variable does not exist", arg->name);
      llvm::Value* loaded_variable;
      llvm::Type *variable_type;
      if(variableIsLocal(arg->name)){
        llvm::AllocaInst* variable = getLocalVariable(arg->name);
        loaded_variable = LLVMBuilder->CreateLoad(variable, "printarg");
        variable_type = variable->getAllocatedType();
      }
      else if(variableIsGlobal(arg->name)){
        llvm::GlobalVariable *globalvariable = getGlobalVariable(arg->name);
        variable_type = globalvariable->getType()->getElementType();
        loaded_variable = LLVMBuilder->CreateLoad(globalvariable, "printarg");
      }
      // Construct the string
      if(variable_type->isFP128Ty())
        print_string += "%f";
      else if(variable_type->isDoubleTy())
        print_string += "%f";
      else if(variable_type->isFloatingPointTy()){
        loaded_variable = createCastLLVM(loaded_variable, llvm::Type::getDoubleTy(*LLVMContext));
        print_string += "%f";
      }
      else if(variable_type->isIntegerTy(128))
        print_string += "%lld";
      else if(variable_type->isIntegerTy(64))
        print_string += "%lld";
      else if(variable_type->isIntegerTy(8))
        print_string += "%d";
      else if(variable_type->isIntegerTy())
        print_string += "%d";
      arguments.push_back(loaded_variable);
    }
  }
  print_string += "\n";
  llvm::Value *print_str = LLVMBuilder->CreateGlobalStringPtr(print_string, "print_str");
  arguments.insert(arguments.begin(), print_str);
  return LLVMBuilder->CreateCall(printfFunc, arguments, "printtmp");
}

/*****************************************************************************/
/******************* Methods related to scan function ***********************/

ScanFunctionCall::ScanFunctionCall(){
  this->args = NULL;
  printf(">>>>>ScanFunctionCall\n");
}

llvm::Value* ScanFunctionCall::codegen() {
  std::cout<< "SCAN FUNCTION" << std::endl;

  std::vector<llvm::Type *> scanArgsTypes({llvm::Type::getInt8PtrTy(*LLVMContext)});
  llvm::FunctionType *scanType = llvm::FunctionType::get(LLVMBuilder->getInt32Ty(),
                                                          scanArgsTypes, true);
  llvm::FunctionCallee scanFunc = LLVMModule->getOrInsertFunction("__isoc99_scanf", scanType);

  std::string scan_string = "";
  std::vector<llvm::Value *> arguments;
  std::map<llvm::Value *, llvm::Value *> convert_from32int_to8int;
  for(auto i = this->args->iterator(); i->hasNext();){
    Argument *arg = i->next();
    // Get the arguments
    if(!variableExists(arg->name))
      error("Variable does not exist", arg->name);
    llvm::Value* variable_parameter;
    llvm::Type *variable_type;
    if(variableIsLocal(arg->name)){
      llvm::AllocaInst* variable = getLocalVariable(arg->name);
      variable_type = variable->getAllocatedType();
      if(variable_type->isIntegerTy(8)){
        llvm::Function *F = LLVMBuilder->GetInsertBlock()->getParent();
        llvm::Type *T =  llvm::Type::getInt32Ty(*LLVMContext);
        llvm::AllocaInst *alloca = createEntryBlockAlloca(F, "scanargcastfrom", T);
        convert_from32int_to8int[alloca] = variable;
        variable_parameter = alloca;
      }
      else{
        variable_parameter = variable;
      }
    }
    else if(variableIsGlobal(arg->name)){
      llvm::GlobalVariable *globalvariable = getGlobalVariable(arg->name);
      variable_type = globalvariable->getType()->getElementType();
      variable_parameter = globalvariable;
    }
    // Construct the string
    if(!scan_string.empty()) scan_string += " ";
    if(variable_type->isFP128Ty())
      scan_string += "%lf";
    else if(variable_type->isDoubleTy())
      scan_string += "%lf";
    else if(variable_type->isFloatingPointTy())
      scan_string += "%f";
    else if(variable_type->isIntegerTy(128))
      scan_string += "%lld";
    else if(variable_type->isIntegerTy(64))
      scan_string += "%lld";
    else if(variable_type->isIntegerTy(16))
      scan_string += "%hd";
    else if(variable_type->isIntegerTy(8))
      scan_string += "%d";
    else if(variable_type->isIntegerTy())
      scan_string += "%d";
    arguments.push_back(variable_parameter);

  }
  //scan_string += "\n";
  llvm::Value *scan_str = LLVMBuilder->CreateGlobalStringPtr(scan_string, "scan_str");
  arguments.insert(arguments.begin(), scan_str);
  LLVMBuilder->CreateCall(scanFunc, arguments);

  // Convert back to int8
  for(auto v = convert_from32int_to8int.begin(); v != convert_from32int_to8int.end(); v++){
    llvm::Type *T =  llvm::Type::getInt8Ty(*LLVMContext);
    llvm::Value *variable_to_cast = LLVMBuilder->CreateLoad(v->first, "scanargcastto_");
    llvm::Value *variable_value = createCastLLVM(variable_to_cast, T);
    LLVMBuilder->CreateStore(variable_value, v->second);
  }

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
  llvm::Function *F = LLVMBuilder->GetInsertBlock()->getParent();
  llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*LLVMContext, "then", F);
  llvm::BasicBlock *EndBB = llvm::BasicBlock::Create(*LLVMContext, "endif");
  llvm::BasicBlock *ElseBB, *ElifCondBB, *ElifBB, *NextBB;

  if(this->ei){
    ElifCondBB = llvm::BasicBlock::Create(*LLVMContext, "elifcond");
    LLVMBuilder->CreateCondBr(logic, ThenBB, ElifCondBB);
    if(this->e) ElseBB = llvm::BasicBlock::Create(*LLVMContext, "else");
  }
  else if(this->e){
    ElseBB = llvm::BasicBlock::Create(*LLVMContext, "else");
    LLVMBuilder->CreateCondBr(logic, ThenBB, ElseBB);
  }
  else
    LLVMBuilder->CreateCondBr(logic, ThenBB, EndBB);

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
      
      ElifBB = llvm::BasicBlock::Create(*LLVMContext, "elif");
      if(i->hasNext()){
        ElifCondBB = llvm::BasicBlock::Create(*LLVMContext, "elifcond");
      }
      else if(this->e) ElifCondBB = ElseBB;
      else ElifCondBB = EndBB;
      LLVMBuilder->CreateCondBr(eliflogic, ElifBB, ElifCondBB);

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
  return nullptr;
}

ForStatement::ForStatement(){
  this->init = NULL;
  this->control = NULL;
  this->iter = NULL;
  this->body = NULL;
  printf(">>>>>ForStatement\n");
}

llvm::Value* ForStatement::codegen() {
  std::cout<< "FOR STATEMENT" << std::endl;
  llvm::Function *F = LLVMBuilder->GetInsertBlock()->getParent();

  llvm::BasicBlock *PreheaderBB = LLVMBuilder->GetInsertBlock();
  llvm::BasicBlock *CondBB = llvm::BasicBlock::Create(*LLVMContext, "forcond", F);
  llvm::BasicBlock *ForBB = llvm::BasicBlock::Create(*LLVMContext, "for");
  llvm::BasicBlock *EndBB = llvm::BasicBlock::Create(*LLVMContext, "endfor");
  llvm::Value *init = this->init->codegen();
  LLVMBuilder->CreateBr(CondBB);
  LLVMBuilder->SetInsertPoint(CondBB);
  llvm::Value *control = this->control->codegen();
  LLVMBuilder->CreateCondBr(control, ForBB, EndBB);

  F->getBasicBlockList().push_back(ForBB);
  LLVMBuilder->SetInsertPoint(ForBB);
  this->body->codegen();
  llvm::Value *iter = this->iter->codegen();
  LLVMBuilder->CreateBr(CondBB);

  F->getBasicBlockList().push_back(EndBB);
  LLVMBuilder->SetInsertPoint(EndBB);
  return nullptr;
}

WhileStatement::WhileStatement(){
  this->control = NULL;
  this->body = NULL;
  printf(">>>>>WhileStatement\n");
}

llvm::Value* WhileStatement::codegen() {
  std::cout<< "WHILE STATEMENT" << std::endl;
  llvm::Function *F = LLVMBuilder->GetInsertBlock()->getParent();

  llvm::BasicBlock *PreheaderBB = LLVMBuilder->GetInsertBlock();
  llvm::BasicBlock *CondBB = llvm::BasicBlock::Create(*LLVMContext, "whilecond", F);
  llvm::BasicBlock *WhileBB = llvm::BasicBlock::Create(*LLVMContext, "while");
  llvm::BasicBlock *EndBB = llvm::BasicBlock::Create(*LLVMContext, "endwhile");
  LLVMBuilder->CreateBr(CondBB);
  LLVMBuilder->SetInsertPoint(CondBB);
  llvm::Value *control = this->control->codegen();
  LLVMBuilder->CreateCondBr(control, WhileBB, EndBB);

  F->getBasicBlockList().push_back(WhileBB);
  LLVMBuilder->SetInsertPoint(WhileBB);
  this->body->codegen();
  LLVMBuilder->CreateBr(CondBB);

  F->getBasicBlockList().push_back(EndBB);
  LLVMBuilder->SetInsertPoint(EndBB);
  return nullptr;
}

DoWhileStatement::DoWhileStatement(){
  this->control = NULL;
  this->body = NULL;
  printf(">>>>>DoWhileStatement\n");
}

llvm::Value* DoWhileStatement::codegen() {
  std::cout<< "DO WHILE STATEMENT" << std::endl;
  llvm::Function *F = LLVMBuilder->GetInsertBlock()->getParent();

  llvm::BasicBlock *PreheaderBB = LLVMBuilder->GetInsertBlock();
  llvm::BasicBlock *CondBB = llvm::BasicBlock::Create(*LLVMContext, "dowhilecond");
  llvm::BasicBlock *DoWhileBB = llvm::BasicBlock::Create(*LLVMContext, "dowhile", F);
  llvm::BasicBlock *EndBB = llvm::BasicBlock::Create(*LLVMContext, "enddowhile");
  LLVMBuilder->CreateBr(DoWhileBB);
  LLVMBuilder->SetInsertPoint(DoWhileBB);
  this->body->codegen();
  LLVMBuilder->CreateBr(CondBB);

  F->getBasicBlockList().push_back(CondBB);
  LLVMBuilder->SetInsertPoint(CondBB);
  llvm::Value *control = this->control->codegen();
  LLVMBuilder->CreateCondBr(control, DoWhileBB, EndBB);

  F->getBasicBlockList().push_back(EndBB);
  LLVMBuilder->SetInsertPoint(EndBB);
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
  fp = NULL;
  fs = NULL;
  vd = NULL;
  s = NULL;
  a = NULL;
  printf(">>>>>StatementComponent\n");
}
llvm::Value* StatementComponent::codegen() {
  std::cout<< "STATEMENT COMPONENT" << std::endl;
  if(fc) fc->codegen();
  else if(fr) fr->codegen();
  else if(fp) fp->codegen();
  else if(fs) fs->codegen();
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

