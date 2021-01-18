#include "llvm.h"

void InitializeModule() {
  // Open a new context and module.
  LLVMContext = std::make_unique<llvm::LLVMContext>();
  LLVMModule = std::make_unique<llvm::Module>("MS", *LLVMContext);

  // Create a new builder for the module.
  LLVMBuilder = std::make_unique<llvm::IRBuilder<>>(*LLVMContext);

  // Create a new pass manager attached to it.
  LLVMFPM = std::make_unique<llvm::legacy::FunctionPassManager>(LLVMModule.get());

  // Promote allocas to registers.
  LLVMFPM->add(llvm::createPromoteMemoryToRegisterPass());
  // Do simple "peephole" optimizations and bit-twiddling optzns.
  LLVMFPM->add(llvm::createInstructionCombiningPass());
  // Reassociate expressions.
  LLVMFPM->add(llvm::createReassociatePass());
  // Eliminate Common SubExpressions.
  LLVMFPM->add(llvm::createGVNPass());
  // Simplify the control flow graph (deleting unreachable blocks, etc).
  LLVMFPM->add(llvm::createCFGSimplificationPass());

  LLVMFPM->doInitialization();

  LLVMValuesInits.clear();
  LLVMValuesInits["double"] = (llvm::ConstantData*) llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(0.0));
  LLVMValuesInits["short int"] = (llvm::ConstantData*) llvm::ConstantInt::get(*LLVMContext, llvm::APInt(16, 0));
}

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *F, 
                                                llvm::StringRef varName,
                                                llvm::Type *varType) {
  llvm::IRBuilder<> TmpB(&F->getEntryBlock(),
                          F->getEntryBlock().begin());
  return TmpB.CreateAlloca(varType, nullptr, varName);
}

llvm::Type *getTypeLLVM(std::string type){
  if(type == "double") return llvm::Type::getDoubleTy(*LLVMContext);
  else if(type == "short int") return llvm::Type::getInt16Ty(*LLVMContext);
  return llvm::Type::getDoubleTy(*LLVMContext);
}

/*Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}
*/