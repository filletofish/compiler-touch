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
class ExprVisitor;
class Value;
namespace llvm {};

class AbstractExpression {
public:
    virtual ~AbstractExpression() = default;
    virtual llvm::Value* Accept(ExprVisitor * visitor) = 0;
};

class NumberExpression : public AbstractExpression {
public:
    int value;
    NumberExpression(int value) : value(value) {}
    llvm::Value* Accept(ExprVisitor *visitor) override { return visitor->Visit(this);}
};

class VariableExpession : public AbstractExpression {
public:
    std::string name;
    VariableExpession(const std::string &name) : name(name) {}
    llvm::Value* Accept(ExprVisitor *visitor) override { return visitor->Visit(this);}
};

class AssignExpression : public AbstractExpression {
public:
    std::string varName;
    AbstractExpression *expr;
    
    AssignExpression(const std::string &varName, AbstractExpression *expr): varName(varName),expr(expr) {}
    llvm::Value* Accept(ExprVisitor *visitor) override { return visitor->Visit(this);}
};

class IfExpression : public AbstractExpression {
public:
    AbstractExpression *conditionExp, *thenExp, *elseExp;

    IfExpression(AbstractExpression *conditionExp, AbstractExpression *thenExp, AbstractExpression *elseExp): conditionExp(conditionExp), thenExp(thenExp), elseExp(elseExp)  {}
    llvm::Value* Accept(ExprVisitor *visitor) override { return visitor->Visit(this);}
};

class ForExpression: public AbstractExpression {
public:
    std::string indexName;
    AbstractExpression *start, *end, *step, *body;
    
    ForExpression(std::string indexName, AbstractExpression *start, AbstractExpression *end, AbstractExpression *step, AbstractExpression *body): indexName(indexName),start(start), end(end), step(step), body(body) {}
    llvm::Value* Accept(ExprVisitor *visitor) override { return visitor->Visit(this);}
};

class BinaryExpression: public AbstractExpression {
public:
    char op;
    AbstractExpression *lhs, *rhs;
    BinaryExpression(char op, AbstractExpression *lhs, AbstractExpression *rhs): op(op), lhs(lhs), rhs(rhs) {}
    llvm::Value* Accept(ExprVisitor *visitor) override { return  visitor->Visit(this);}
};
#endif /* AbstractExpresssion_hpp */

