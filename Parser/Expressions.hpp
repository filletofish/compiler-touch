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
#include "../IRGeneration/AbstractVisitor.hpp"

class Value;
namespace llvm {};

class AbstractExpression {
public:
    virtual std::string stringValue() {return "";};
    
    virtual ~AbstractExpression() = default;
    virtual void Accept(AbstractVisitor * visitor) = 0;
};

class NumberExpression : public AbstractExpression {
public:
    int value;
    std::string stringValue() override { return std::to_string(value); }
    NumberExpression(int value) : value(value) {};
    void Accept(AbstractVisitor *visitor) override { visitor->Visit(this);}
};

class VariableExpession : public AbstractExpression {
private:
    int _SSAIndex;
public:
    std::string name;
    void SetSSAIndex(int index) { _SSAIndex = index; };
    std::string stringValue() override { return name + "_" + std::to_string(_SSAIndex); }
    
    VariableExpession(const std::string &name) : name(name), _SSAIndex(0) {}
    
    void Accept(AbstractVisitor *visitor) override { visitor->Visit(this);}
};

class AssignExpression : public AbstractExpression {
public:
    std::string varName() { return varExp ->name; };
    AbstractExpression *expr;
    VariableExpession *varExp;
    
    AssignExpression(VariableExpession *varExp, AbstractExpression *expr): varExp(varExp),expr(expr) {}
    void Accept(AbstractVisitor *visitor) override { visitor->Visit(this);}
};

class IfExpression : public AbstractExpression {
public:
    AbstractExpression *conditionExp, *thenExp, *elseExp;

    IfExpression(AbstractExpression *conditionExp, AbstractExpression *thenExp, AbstractExpression *elseExp): conditionExp(conditionExp), thenExp(thenExp), elseExp(elseExp)  {}
    void Accept(AbstractVisitor *visitor) override { visitor->Visit(this);}
};

class ForExpression: public AbstractExpression {
public:
    VariableExpession *index;
    AbstractExpression *start, *end, *step, *body;
    
    ForExpression(std::string indexName, AbstractExpression *start, AbstractExpression *end, AbstractExpression *step, AbstractExpression *body): index(new VariableExpession(indexName)), start(start), end(end), step(step), body(body) {}
    void Accept(AbstractVisitor *visitor) override { visitor->Visit(this);}
};

class BinaryExpression: public AbstractExpression {
public:
    char op;
    AbstractExpression *lhs, *rhs;
    std::string stringValue() override { return lhs->stringValue() + " " + op + " " + rhs->stringValue(); };

    
    BinaryExpression(char op, AbstractExpression *lhs, AbstractExpression *rhs): op(op), lhs(lhs), rhs(rhs) {}
    void Accept(AbstractVisitor *visitor) override { visitor->Visit(this);}
};
#endif /* AbstractExpresssion_hpp */

