//
//  main.cpp
//  Compiler
//
//  Created by Филипп Федяков on 25.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Parser.hpp"
#include "Lexer.hpp"
#include "IRGeneration.hpp"
#include "Expressions.hpp"


//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
using namespace llvm;

int main(int argc, const char * argv[]) {
    if (argc > 1)
    {
        FILE * fp = freopen(argv[1], "r", stdin);
        if (fp == NULL)
        {
            perror(argv[1]);
            exit(1);
        }
    }
    
    Lexer *lexer = new Lexer();
    Parser *parser = new Parser(lexer);
    std::vector<AbstractExpression *> a = parser->Parse();
    
    
    
    LLVMContext context;
    IRBuilder<> Builder(context);
    
    
//     Make the module, which holds all the code.
    Module *module = new Module("My Module", context);
    Function *mainFunction = module->getFunction("main");
    FunctionType *FT = FunctionType::get(Builder.getInt32Ty(),false);
    mainFunction = Function::Create(FT, GlobalValue::CommonLinkage, "main", module);
    
    BasicBlock *BB = BasicBlock::Create(context, "entrypoint", mainFunction);
    Builder.SetInsertPoint(BB);
    
    CodeGenVisitor codeGenVisitor = CodeGenVisitor(&context, &Builder);
    llvm::Value *value = nullptr;
    for (std::vector<AbstractExpression *>::iterator it = a.begin(); it != a.end(); ++it)
        value = (*it)->Accept(&codeGenVisitor);

    
    Builder.CreateRet(value);

    module->dump();
    return 0;
}
