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
#include "LLVMIRGenerator.hpp"
#include "Expressions.hpp"
#include "IRGenerator.hpp"
#include "GraphVizPrinter.hpp"



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
    std::vector<AbstractExpression *> expressions = parser->Parse();
    
    
    if (/* DISABLES CODE */ (0)) {
        LLVMContext context;
        IRBuilder<> Builder(context);
        
        
        //     Make the module, which holds all the code.
        Module *module = new Module("My Module", context);
        Function *mainFunction = module->getFunction("main");
        FunctionType *FT = FunctionType::get(Builder.getInt32Ty(),false);
        mainFunction = Function::Create(FT, GlobalValue::CommonLinkage, "main", module);
        
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entrypoint", mainFunction);
        Builder.SetInsertPoint(BB);
        
        LLVMIRGenerator llvmIRGenerator = LLVMIRGenerator(&context, &Builder);
        llvm::Value *value = nullptr;
        for (std::vector<AbstractExpression *>::iterator it = expressions.begin(); it != expressions.end(); ++it)
            value = llvmIRGenerator.GenerateIR((*it));
        
        
        Builder.CreateRet(value);
        
        module->dump();
    } else {
        IRGenerator irGenerator = IRGenerator();
        for (std::vector<AbstractExpression *>::iterator it = expressions.begin(); it != expressions.end(); ++it)
            irGenerator.GenerateIR((*it));
        
        
        irGenerator.CommitBuildingAndDump();
        irGenerator.GetGraphVizPrinter().print();
    }
    
    return 0;
}
