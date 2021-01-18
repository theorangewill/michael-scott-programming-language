#ifndef __llvm_h
#define __llvm_h

#include "main.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

static std::unique_ptr<llvm::LLVMContext> LLVMContext;
static std::unique_ptr<llvm::Module> LLVMModule;
static std::unique_ptr<llvm::IRBuilder<>> LLVMBuilder;
static std::unique_ptr<llvm::legacy::FunctionPassManager> LLVMFPM;
static std::map<std::string, llvm::ConstantData*> LLVMValuesInits;


static void InitializeModule() {
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

static llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *F, 
                                                llvm::StringRef varName,
                                                llvm::Type *varType) {
  llvm::IRBuilder<> TmpB(&F->getEntryBlock(),
                          F->getEntryBlock().begin());
  return TmpB.CreateAlloca(varType, nullptr, varName);
}

static llvm::Type *getTypeLLVM(std::string type){
  if(type == "double") return llvm::Type::getDoubleTy(*LLVMContext);
  else if(type == "short int") return llvm::Type::getInt16Ty(*LLVMContext);
  return llvm::Type::getDoubleTy(*LLVMContext);
}

/*Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}
*/
#endif // __llvm_h