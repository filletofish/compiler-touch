//
//  IRStatements.hpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef IRInstructions_hpp
#define IRInstructions_hpp

#include <stdio.h>
#include <string>
#include <map>
#include "AbstractVisitor.hpp"

class AbstractExpression;
class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class ControlFlowGraph;
class BasicBlock;

enum StatementType {
    ASSIGN = 0,
    BRANCH = 1,
    PHI = 2
};

class AbstractStatement {
public:
    StatementType type;
    virtual std::string Dump() = 0;
    AbstractStatement(StatementType type) : type(type) {};
    virtual void Accept(AbstractVisitor * visitor) = 0;
};


class AssignStatement: public AbstractStatement {
public:
    VariableExpession *var;
    AbstractExpression *rhs;
    
    std::string Dump() override;
    AssignStatement(VariableExpession *var, AbstractExpression *rhs) : AbstractStatement(ASSIGN), var(var), rhs(rhs) {};
    void Accept(AbstractVisitor *visitor) override { visitor->Visit(this);}
};

class BranchStatement: public AbstractStatement {
    
private:
    BranchStatement (AbstractExpression *condition, BasicBlock *firstBranchBB, BasicBlock *secondBranchBB, bool isCond) : AbstractStatement(BRANCH), condition(condition), firstBranchBB(firstBranchBB), secondBranchBB(secondBranchBB), isConditional(isCond) {};
public:
    bool isConditional;
    AbstractExpression *condition;
    BasicBlock *firstBranchBB;
    BasicBlock *secondBranchBB;
    std::string Dump() override;
    BranchStatement (AbstractExpression *condition, BasicBlock *trueBranch, BasicBlock *falseBranch) : BranchStatement(condition, trueBranch, falseBranch, true) {};
    BranchStatement (BasicBlock *bb) : BranchStatement(nullptr, bb, nullptr, false) {};
    void Accept(AbstractVisitor *visitor) override { visitor->Visit(this);}
};


class PhiNodeStatement: public AbstractStatement {
public:
    VariableExpession *var;
    std::map<BasicBlock *, VariableExpession *> bbToVarMap;
    std::string Dump() override;
    PhiNodeStatement (VariableExpession *var, std::map<BasicBlock *, VariableExpession *> bbToVarMap) : bbToVarMap(bbToVarMap), var(var), AbstractStatement(PHI) {};
    void Accept(AbstractVisitor *visitor) override { }
};

#endif /* IRInstructions_hpp */
