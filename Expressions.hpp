//
//  AbstractExpresssion.hpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef AbstractExpresssion_hpp
#define AbstractExpresssion_hpp

#include <stdio.h>
#include <string>
#include "IRGeneration.hpp"
#include "CustomIRGeneration.hpp"

class Value;
namespace llvm {};

class AbstractExpression {
public:
    std::string stringValue = "";
    virtual ~AbstractExpression() = default;
    virtual llvm::Value* Accept(IRLLVMGenerationVisitor * visitor) = 0;
    virtual int* Accept(CustomIRGenerationVisitor * visitor) = 0;
};

class NumberExpression : public AbstractExpression {
public:
    int value;
    NumberExpression(int value) : value(value) { stringValue = std::to_string(value); }
    llvm::Value* Accept(IRLLVMGenerationVisitor *visitor) override { return visitor->Visit(this);}
    int* Accept(CustomIRGenerationVisitor * visitor) override { return visitor->Visit(this);}
};

class VariableExpession : public AbstractExpression {
public:
    std::string name;
    VariableExpession(const std::string &name) : name(name) { stringValue = name; }
    llvm::Value* Accept(IRLLVMGenerationVisitor *visitor) override { return visitor->Visit(this);}
    int* Accept(CustomIRGenerationVisitor * visitor) override { return visitor->Visit(this);}
};

class AssignExpression : public AbstractExpression {
public:
    std::string varName() { return varExp ->name; };
    AbstractExpression *expr;
    VariableExpession *varExp;
    
    
    AssignExpression(VariableExpession *varExp, AbstractExpression *expr): varExp(varExp),expr(expr) {}
    llvm::Value* Accept(IRLLVMGenerationVisitor *visitor) override { return visitor->Visit(this);}
    int* Accept(CustomIRGenerationVisitor * visitor) override { return visitor->Visit(this);}
};

class IfExpression : public AbstractExpression {
public:
    AbstractExpression *conditionExp, *thenExp, *elseExp;

    IfExpression(AbstractExpression *conditionExp, AbstractExpression *thenExp, AbstractExpression *elseExp): conditionExp(conditionExp), thenExp(thenExp), elseExp(elseExp)  {}
    llvm::Value* Accept(IRLLVMGenerationVisitor *visitor) override { return visitor->Visit(this);}
    int* Accept(CustomIRGenerationVisitor * visitor) override { return visitor->Visit(this);}
};

class ForExpression: public AbstractExpression {
public:
    std::string indexName;
    AbstractExpression *start, *end, *step, *body;
    
    ForExpression(std::string indexName, AbstractExpression *start, AbstractExpression *end, AbstractExpression *step, AbstractExpression *body): indexName(indexName),start(start), end(end), step(step), body(body) {}
    llvm::Value* Accept(IRLLVMGenerationVisitor *visitor) override { return visitor->Visit(this);}
    int* Accept(CustomIRGenerationVisitor * visitor) override { return visitor->Visit(this);}
};

class BinaryExpression: public AbstractExpression {
public:
    char op;
    AbstractExpression *lhs, *rhs;
    BinaryExpression(char op, AbstractExpression *lhs, AbstractExpression *rhs): op(op), lhs(lhs), rhs(rhs) { stringValue = lhs->stringValue + op + rhs->stringValue; }
    llvm::Value* Accept(IRLLVMGenerationVisitor *visitor) override { return  visitor->Visit(this);}
    int* Accept(CustomIRGenerationVisitor * visitor) override { return visitor->Visit(this);}
};
#endif /* AbstractExpresssion_hpp */

