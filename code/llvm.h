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
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#ifndef __iostream
    #define __iostream
    #include <iostream>
#endif

extern std::unique_ptr<llvm::LLVMContext> LLVMContext;
extern std::unique_ptr<llvm::Module> LLVMModule;
extern std::unique_ptr<llvm::IRBuilder<>> LLVMBuilder;
extern std::unique_ptr<llvm::legacy::FunctionPassManager> LLVMFPM;
extern std::map<std::string, llvm::Constant*> LLVMValuesInits;

void InitializeModule();
void CreateObjectFile(std::string output_file);
llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *F, llvm::StringRef varName, llvm::Type *varType);
llvm::Type *getTypeLLVM(std::string type);
llvm::Value *createCastLLVM(llvm::Value* variable, llvm::Type *type_to_be_casted);
llvm::Value* negativeExpression(llvm::Type *current_type, llvm::Value *temp);

#endif // __llvm_h