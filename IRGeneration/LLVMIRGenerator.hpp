//
//  CodeGenVisitor.hpp
//  Compiler
//
//  Created by Филипп Федяков on 28.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef CodeGenVisitor_hpp
#define CodeGenVisitor_hpp

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

#include "AbstractVisitor.hpp"

#include <stdio.h>
#include <string>
#include <algorithm>

class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class AbstractExpression;
class ControlFlowGraph;
class BasicBlock;

class LLVMIRGenerator : public AbstractVisitor {
public:
    void Visit(NumberExpression *exp) override;
    void Visit(VariableExpession *exp) override;
    void Visit(AssignExpression *exp) override;
    void Visit(IfExpression *exp) override;
    void Visit(ForExpression *exp) override;
    void Visit(BinaryExpression *exp) override;
    
    // not implemented, because visits only expressions
    virtual void Visit(BranchStatement *stmt) override {};
    virtual void Visit(AssignStatement *stmt) override {};
    
    llvm::Value* GenerateIR(AbstractExpression *exp);
    
    LLVMIRGenerator(llvm::LLVMContext *TheContext,
                   llvm::IRBuilder<> *Builder) : TheContext(TheContext), Builder(Builder){};
    
private:
    llvm::Value* _latestValue;
    llvm::LLVMContext *TheContext;
    llvm::IRBuilder<> *Builder;
    std::map<std::string, llvm::AllocaInst *> namedValues;    
    void LogError(const char*);
};
#endif /* CodeGenVisitor_hpp */
