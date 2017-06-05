//
//  CustomIRGeneration.hpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef CustomIRGeneration_hpp
#define CustomIRGeneration_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <set>

#include "AbstractVisitor.hpp"

class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class ControlFlowGraph;
class BasicBlock;
class AbstractExpression;
class AbstractStatement;
class BranchStatement;
class AssignStatement;


class CustomIRGenerationVisitor : public AbstractVisitor {
private:
    std::map<std::string, std::set<BasicBlock *>> bblocksForVar;
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
    
    int GenerateIR(AbstractExpression *exp);
    void InsertPhiNodes();
    void BuildSSAForm();
    void Dump();
    CustomIRGenerationVisitor(ControlFlowGraph *cfg);
    
private:
    int _latestValue;
    std::map<std::string, int*> namedValues;
    BasicBlock *currentBB;
    BasicBlock *entryBB;
    BasicBlock *CreateBB(std::string label);
    void CreateBr(BasicBlock *targetBB);
    void CreateConditionalBr(AbstractExpression *condition, BasicBlock *thenBB, BasicBlock *elseBB);
    ControlFlowGraph *cfg;
    void LogError(const char*);
};

class VarSearchVisitor : public AbstractVisitor {
private:
    std::set<VariableExpession *> vars;
public:
    void Visit(NumberExpression *exp) override;
    void Visit(VariableExpession *exp) override;
    void Visit(AssignExpression *exp) override;
    void Visit(IfExpression *exp) override;
    void Visit(ForExpression *exp) override;
    void Visit(BinaryExpression *exp) override;
    virtual void Visit(BranchStatement *stmt) override;
    virtual void Visit(AssignStatement *stmt) override;
    std::set<VariableExpession *> AllVarsUsedInStatement(AbstractStatement *statement);
};

class SSAFormer {
private:
    int counter;
    std::vector<int> stack;
    VarSearchVisitor varSearcher = VarSearchVisitor();
    void TraverseBBWithVar(BasicBlock *bb, std::string varName);
    ControlFlowGraph *cfg;
public:
    void RenameVarToSSAForm(std::string varName);
    SSAFormer(ControlFlowGraph *cfg) : cfg(cfg) {};
};
#endif /* CustomIRGeneration_hpp */
