#include "llvm.h"

std::unique_ptr<llvm::LLVMContext> LLVMContext;
std::unique_ptr<llvm::Module> LLVMModule;
std::unique_ptr<llvm::IRBuilder<>> LLVMBuilder;
std::unique_ptr<llvm::legacy::FunctionPassManager> LLVMFPM;
std::map<std::string, llvm::Constant*> LLVMValuesInits;

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
  LLVMValuesInits["128int"] = llvm::ConstantInt::get(llvm::Type::getInt128Ty(*LLVMContext), 0.0, true);
  LLVMValuesInits["64int"] = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*LLVMContext), 0.0, true);
  LLVMValuesInits["32int"] = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*LLVMContext), 0.0, true);
  LLVMValuesInits["16int"] = llvm::ConstantInt::get(llvm::Type::getInt16Ty(*LLVMContext), 0.0, true);
  LLVMValuesInits["8int"] = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*LLVMContext), 0.0, true);
  LLVMValuesInits["1int"] = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*LLVMContext), 0.0);
  LLVMValuesInits["u128int"] = llvm::ConstantInt::get(llvm::Type::getInt128Ty(*LLVMContext), 0.0);
  LLVMValuesInits["u64int"] = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*LLVMContext), 0.0);
  LLVMValuesInits["u32int"] = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*LLVMContext), 0.0);
  LLVMValuesInits["u16int"] = llvm::ConstantInt::get(llvm::Type::getInt16Ty(*LLVMContext), 0.0);
  LLVMValuesInits["u8int"] = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*LLVMContext), 0.0);
  LLVMValuesInits["128float"] = llvm::ConstantFP::get(llvm::Type::getFP128Ty(*LLVMContext), 0.0);
  LLVMValuesInits["64float"] = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*LLVMContext), 0.0);
  LLVMValuesInits["32float"] = llvm::ConstantFP::get(llvm::Type::getFloatTy(*LLVMContext), 0.0);
  LLVMValuesInits["16float"] = llvm::ConstantFP::get(llvm::Type::getHalfTy(*LLVMContext), 0.0);
  LLVMValuesInits["char"] = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*LLVMContext), 0.0);
  LLVMValuesInits["void"] = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*LLVMContext), 0.0);
}

llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *F, llvm::StringRef varName, llvm::Type *varType) {
  llvm::IRBuilder<> TmpB(&F->getEntryBlock(), F->getEntryBlock().begin());
  return TmpB.CreateAlloca(varType, nullptr, varName);
}

llvm::Type *getTypeLLVM(std::string type){
  if(type == "128float") return llvm::Type::getFP128Ty(*LLVMContext);
  else if(type == "64float") return llvm::Type::getDoubleTy(*LLVMContext);
  else if(type == "32float") return llvm::Type::getFloatTy(*LLVMContext);
  else if(type == "16float") return llvm::Type::getHalfTy(*LLVMContext);
  else if(type == "128int") return llvm::Type::getInt128Ty(*LLVMContext);
  else if(type == "64int") return llvm::Type::getInt64Ty(*LLVMContext);
  else if(type == "32int") return llvm::Type::getInt32Ty(*LLVMContext);
  else if(type == "16int") return llvm::Type::getInt16Ty(*LLVMContext);
  else if(type == "8int") return llvm::Type::getInt8Ty(*LLVMContext);
  else if(type == "1int") return llvm::Type::getInt1Ty(*LLVMContext);
  else if(type == "u128int") return llvm::Type::getInt128Ty(*LLVMContext);
  else if(type == "u64int") return llvm::Type::getInt64Ty(*LLVMContext);
  else if(type == "u32int") return llvm::Type::getInt32Ty(*LLVMContext);
  else if(type == "u16int") return llvm::Type::getInt16Ty(*LLVMContext);
  else if(type == "u8int") return llvm::Type::getInt8Ty(*LLVMContext);
  else if(type == "void") return llvm::Type::getVoidTy(*LLVMContext);
  else if(type == "char") return llvm::Type::getInt8Ty(*LLVMContext);
  return nullptr;
}


llvm::Value *createCastLLVM(llvm::Value* variable, llvm::Type *type_to_be_casted){
  llvm::Instruction::CastOps handle_cast;
  if(variable->getType()->isFloatingPointTy() && type_to_be_casted->isFloatingPointTy()){
    int variable_type = variable->getType()->getTypeID();
    int cast_type = type_to_be_casted->getTypeID();
    if(variable_type == cast_type)
      return variable;
    else if(variable_type < cast_type)
      handle_cast = llvm::Instruction::FPExt;
    else
      handle_cast = llvm::Instruction::FPTrunc;
  }
  else if(variable->getType()->isIntegerTy() && type_to_be_casted->isIntegerTy()){
    unsigned variable_size = llvm::cast<llvm::IntegerType>(variable->getType())->getBitWidth();
    unsigned cast_size = llvm::cast<llvm::IntegerType>(type_to_be_casted)->getBitWidth();
    if(variable_size == cast_size)
      return variable;
    else if(variable_size < cast_size)
      handle_cast = llvm::Instruction::SExt;
    else
      handle_cast = llvm::Instruction::Trunc;
  }
  else if(variable->getType()->isIntegerTy() && type_to_be_casted->isFloatingPointTy())
    handle_cast = llvm::Instruction::SIToFP;
  else if(variable->getType()->isFloatingPointTy() && type_to_be_casted->isIntegerTy())
    handle_cast = llvm::Instruction::FPToSI;
  else if(variable->getType()->isIntegerTy() && type_to_be_casted->isPointerTy())
    handle_cast = llvm::Instruction::IntToPtr;
  else if(variable->getType()->isPointerTy() && type_to_be_casted->isIntegerTy())
    handle_cast = llvm::Instruction::PtrToInt;

  return LLVMBuilder->CreateCast(handle_cast, variable, type_to_be_casted, "cast");
}

llvm::Value* negativeExpression(llvm::Type *current_type, llvm::Value *temp){
    if(current_type->isFP128Ty())
      return LLVMBuilder->CreateFMul(temp, llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(-1.0)), "neg");
    else if(current_type->isDoubleTy())
      return LLVMBuilder->CreateFMul(temp, llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(-1.0)), "neg");
    else if(temp->getType()->isFloatingPointTy())
      return LLVMBuilder->CreateFMul(temp, llvm::ConstantFP::get(*LLVMContext, llvm::APFloat(-1.0f)), "neg");
    else if(temp->getType()->isIntegerTy(128))
      //todo: retornando 18446744073709551615
      return LLVMBuilder->CreateMul(temp, llvm::ConstantInt::get(*LLVMContext, llvm::APInt(128,-1)), "neg");
    else if(temp->getType()->isIntegerTy(64))
      return LLVMBuilder->CreateMul(temp, llvm::ConstantInt::get(*LLVMContext, llvm::APInt(64,-1)), "neg");
    else if(temp->getType()->isIntegerTy())
      return LLVMBuilder->CreateMul(temp, llvm::ConstantInt::get(*LLVMContext, llvm::APInt(32,-1)), "neg");
    else
      return nullptr;
}

void CreateObjectFile(std::string output_file){
  // Initialize the target registry etc.
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  auto TargetTriple = llvm::sys::getDefaultTargetTriple();
  LLVMModule->setTargetTriple(TargetTriple);

  std::string error_message;
  auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, error_message);

  // Print an error and exit if we couldn't find the requested target.
  // This generally occurs if we've forgotten to initialise the
  // TargetRegistry or we have a bogus target triple.
  if (!Target) {
    printf("**Error creating object file: %s\n", error_message.c_str());
    exit(1);
  }

  auto CPU = "generic";
  auto Features = "";

  llvm::TargetOptions opt;
  auto RM = llvm::Optional<llvm::Reloc::Model>();
  auto TheTargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

  LLVMModule->setDataLayout(TheTargetMachine->createDataLayout());

  auto Filename = output_file.c_str();
  std::error_code error_code;
  llvm::raw_fd_ostream dest(Filename, error_code, llvm::sys::fs::OF_None);

  if(error_code){
    printf("**Error creating object file: Could not open file %s\n", error_code.message().c_str());
    exit(1);
  }

  llvm::legacy::PassManager pass;
  auto FileType = llvm::CGFT_ObjectFile;

  if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    printf("**Error TheTargetMachine can't emit a file of this type\n");
    exit(1);
  }

  pass.run(*LLVMModule);
  dest.flush();

  printf("Object file created: %s\n", Filename);
  std::string Str;
  llvm::raw_string_ostream OS(Str);
  LLVMModule->print(OS, nullptr);
  OS.flush();
  std::cout << Str << std::endl;
}
