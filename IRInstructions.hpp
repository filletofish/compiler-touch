//
//  IRInstructions.hpp
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
#include "CustomIRGeneration.hpp"

class AbstractExpression;
class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class ControlFlowGraph;
class BasicBlock;

enum InstructionType {
    ASSIGN = 0,
    RETURN = 1,
    BRANCH = 2,
    PHI = 3
};

class AbstractInstruction {
public:
    InstructionType type;
    virtual std::string Dump() = 0;
    AbstractInstruction(InstructionType type) : type(type) {};
    virtual void Accept(VarSearchVisitor * visitor) = 0;
};


class AssignInstruction: public AbstractInstruction {
public:
    VariableExpession *var;
    AbstractExpression *rhs;
    
    std::string Dump() override;
    AssignInstruction(VariableExpession *var, AbstractExpression *rhs) : AbstractInstruction(ASSIGN), var(var), rhs(rhs) {};
    void Accept(VarSearchVisitor * visitor) override { return visitor->Visit(this);}
};

class BranchInstruction: public AbstractInstruction {
    
private:
    BranchInstruction (AbstractExpression *condition, BasicBlock *firstBranchBB, BasicBlock *secondBranchBB, bool isCond) : AbstractInstruction(BRANCH), condition(condition), firstBranchBB(firstBranchBB), secondBranchBB(secondBranchBB), isConditional(isCond) {};
public:
    bool isConditional;
    AbstractExpression *condition;
    BasicBlock *firstBranchBB;
    BasicBlock *secondBranchBB;
    std::string Dump() override;
    BranchInstruction (AbstractExpression *condition, BasicBlock *trueBranch, BasicBlock *falseBranch) : BranchInstruction(condition, trueBranch, falseBranch, true) {};
    BranchInstruction (BasicBlock *bb) : BranchInstruction(nullptr, bb, nullptr, false) {};
    void Accept(VarSearchVisitor * visitor) override { return visitor->Visit(this);}
};


class PhiInstruction: public AbstractInstruction {
public:
    VariableExpession *var;
    std::map<BasicBlock *, VariableExpession *> bbToVarMap;
    std::string Dump() override;
    PhiInstruction (VariableExpession *var, std::map<BasicBlock *, VariableExpession *> bbToVarMap) : bbToVarMap(bbToVarMap), var(var), AbstractInstruction(PHI) {};
    void Accept(VarSearchVisitor * visitor) override {}
};

#endif /* IRInstructions_hpp */
