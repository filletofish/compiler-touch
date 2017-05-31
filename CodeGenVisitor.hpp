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

#include <stdio.h>
#include <string>
#include <algorithm>

class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;

class ExprVisitor {
public:
    virtual llvm::Value* Visit(NumberExpression *exp) = 0;
    virtual llvm::Value* Visit(VariableExpession *exp) = 0;
    virtual llvm::Value* Visit(AssignExpression *exp) = 0;
    virtual llvm::Value* Visit(IfExpression *exp) = 0;
    virtual llvm::Value* Visit(ForExpression *exp) = 0;
    virtual llvm::Value* Visit(BinaryExpression *exp) = 0;
};

class CodeGenVisitor : public ExprVisitor {
public:
    virtual llvm::Value* Visit(NumberExpression *exp) override;
    virtual llvm::Value* Visit(VariableExpession *exp) override;
    virtual llvm::Value* Visit(AssignExpression *exp) override;
    /// generates IR for if condition
    virtual llvm::Value* Visit(IfExpression *exp) override;
    virtual llvm::Value* Visit(ForExpression *exp) override;
    virtual llvm::Value* Visit(BinaryExpression *exp) override;
    CodeGenVisitor(llvm::LLVMContext *TheContext,
                   llvm::IRBuilder<> *Builder) : TheContext(TheContext), Builder(Builder){};
    
private:
    llvm::LLVMContext *TheContext;
    llvm::IRBuilder<> *Builder;
    std::map<std::string, llvm::AllocaInst *> namedValues;    
    llvm::Value* LogError(const char*);
};


#endif /* CodeGenVisitor_hpp */
