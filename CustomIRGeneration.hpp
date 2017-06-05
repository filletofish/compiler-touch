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

class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class ControlFlowGraph;
class BasicBlock;
class AbstractExpression;
class AbstractInstruction;
class BranchInstruction;
class AssignInstruction;


class CustomIRGenerationVisitor {
private:
    std::map<std::string, std::set<BasicBlock *>> bblocksForVar;
public:
    int Visit(NumberExpression *exp);
    int Visit(VariableExpession *exp);
    int Visit(AssignExpression *exp);
    /// generates IR for if condition
    int Visit(IfExpression *exp);
    int Visit(ForExpression *exp);
    int Visit(BinaryExpression *exp);
    
    void InsertPhiNodes();
    void BuildSSAForm();
    void Dump();
    CustomIRGenerationVisitor(ControlFlowGraph *cfg);
    
private:
    std::map<std::string, int*> namedValues;
    BasicBlock *currentBB;
    BasicBlock *entryBB;
    BasicBlock *CreateBB(std::string label);
    void CreateBr(BasicBlock *targetBB);
    void CreateConditionalBr(AbstractExpression *condition, BasicBlock *thenBB, BasicBlock *elseBB);
    ControlFlowGraph *cfg;
    int LogError(const char*);
};

class VarSearchVisitor {
private:
    std::set<VariableExpession *> vars;
public:
    void Visit(NumberExpression *exp);
    void Visit(VariableExpession *exp);
    void Visit(AssignExpression *exp);
    void Visit(IfExpression *exp);
    void Visit(ForExpression *exp);
    void Visit(BinaryExpression *exp);
    void Visit(BranchInstruction *instr);
    void Visit(AssignInstruction *instr);
    std::set<VariableExpession *> AllVarsUsedInInstruction(AbstractInstruction *instruction);
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
