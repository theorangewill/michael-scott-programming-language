#ifndef __llvm_h
#define __llvm_h

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

void InitializeModule();
llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *F, 
                                                llvm::StringRef varName,
                                                llvm::Type *varType);
llvm::Type *getTypeLLVM(std::string type);

#endif // __llvm_h