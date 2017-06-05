//
//  CustomIRGeneration.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "IRGenerator.hpp"
#include "BasicBlock.hpp"
#include "Expressions.hpp"
#include "IRStatements.hpp"
#include "ControlFlowGraph.hpp"
#include <assert.h> 

using namespace std;

BasicBlock* IRGenerator::CreateBB(string label) {
    int nextIndex = (int)cfg->basicBlocks.size();
    BasicBlock *bb = new BasicBlock(nextIndex, label);
    cfg->AddBasicBlock(bb);
    return bb;
}

IRGenerator::IRGenerator(ControlFlowGraph *cfg) :cfg(cfg) {
    BasicBlock *bb = CreateBB("entry");
    currentBB = bb;
    entryBB = bb;
}

void IRGenerator::CreateBr(BasicBlock *targetBB) {
    BasicBlock::AddLink(currentBB, targetBB);
    BranchStatement *branchStmt = new BranchStatement(targetBB);
    currentBB->AddStatement(branchStmt);
}

void IRGenerator::CreateConditionalBr(AbstractExpression *condition, BasicBlock *thenBB, BasicBlock *elseBB) {
    BasicBlock::AddLink(currentBB, thenBB);
    BasicBlock::AddLink(currentBB, elseBB);
    BranchStatement *branchStmt = new BranchStatement(condition, thenBB, elseBB);
    currentBB->AddStatement(branchStmt);
}

int IRGenerator::GenerateIR(AbstractExpression *exp) {
    exp->Accept(this);
    return _latestValue;
}

void IRGenerator::Visit(NumberExpression *exp) {
    _latestValue = exp->value;
}

void IRGenerator::Visit(VariableExpession *exp) {
    int* value = namedValues[exp->name];
    if (!value)
        return LogError("Unknown variable name");
    _latestValue = *value;
}

void IRGenerator::Visit(AssignExpression *exp) {
    bblocksForVar[exp->varExp->name].insert(currentBB);
    int assignValue = GenerateIR(exp->expr);
    namedValues[exp->varName()] = &assignValue;
    currentBB->AddStatement(new AssignStatement(exp->varExp, exp->expr));
    _latestValue = assignValue;
}

void IRGenerator::Visit(IfExpression *exp) {
    BasicBlock *thenBB = CreateBB("then");
    BasicBlock *elseBB = CreateBB("else");
    BasicBlock *mergeBB = CreateBB("if_cont");
    
    CreateConditionalBr(exp->conditionExp, elseBB, thenBB);
    
    currentBB = thenBB;
    GenerateIR(exp->thenExp);
    CreateBr(mergeBB);
    
    currentBB = elseBB;
    GenerateIR(exp->elseExp);
    CreateBr(mergeBB);
    
    currentBB = mergeBB;
    
    _latestValue = 0;
}

void IRGenerator::Visit(ForExpression *exp) {
    // TODO: Refactor to make less pseudo steps and to copy var instead of creating new
    
    bblocksForVar[exp->index->name].insert(currentBB);
    // Emit the start code first, without 'variable' in scope.
    int startVal = GenerateIR(exp->start);
    VariableExpession *pseudoVarForStart = new VariableExpession(exp->index->name);
    currentBB->AddStatement(new AssignStatement(pseudoVarForStart, exp->start));

    int *oldVal = namedValues[exp->index->name];
    namedValues[exp->index->name] = &startVal;
    
    
    BasicBlock *loopCoonditionBB = CreateBB("loop_cond");
    CreateBr(loopCoonditionBB);
    
    currentBB = loopCoonditionBB;
    

    // Compute the end condition.
    bblocksForVar[exp->index->name].insert(currentBB);
    GenerateIR(exp->end);
    // Make the new basic block for the loop body
    BasicBlock *loopBodyBB = CreateBB("loop_body");
    BasicBlock *loopAfterBB = CreateBB("loop_cont");
    // MARK: Make pseudo step it in expression
    VariableExpession *pseudoVarForConditionCheck = new VariableExpession(exp->index->name);
    BinaryExpression *pseudoCompExp = new BinaryExpression('-', exp->end, pseudoVarForConditionCheck);
    
    CreateConditionalBr(pseudoCompExp, loopBodyBB, loopAfterBB);
    
    // Start insertion in LoopBB.
    currentBB = loopBodyBB;
    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    GenerateIR(exp->body);
    
    // MARK: Make pseudo step it in expression
    bblocksForVar[exp->index->name].insert(currentBB);
    BinaryExpression *pseudoStepExp = new BinaryExpression('+', new VariableExpession(exp->index->name), new NumberExpression(1));
    currentBB->AddStatement(new AssignStatement(exp->index, pseudoStepExp));
    
    CreateBr(loopCoonditionBB);

    // Any new code will be inserted in AfterBB.
    currentBB = loopAfterBB;
    
    // Restore the unshadowed variable.
    if (oldVal)
        namedValues[exp->index->name] = oldVal;
    else
        namedValues.erase(exp->index->name);
    
    // for expr always returns 0.
    _latestValue = 0;
}

void IRGenerator::Visit(BinaryExpression *exp) {
    
    int lhsValue = GenerateIR(exp->lhs);
    int rhsValue = GenerateIR(exp->rhs);
    
    switch (exp->op) {
        case '+':
            _latestValue = lhsValue + rhsValue;
            return;
        case '-':
            _latestValue = lhsValue - rhsValue;
            return;
        default:
            LogError("invalid binary operator");
            return;
    };
}

void IRGenerator::LogError(const char * str) {
    fprintf(stderr, "Error: %s\n", str);
    _latestValue = 0;
}


void IRGenerator::CommitBuildingAndDump() {
    cfg->CommitAllChanges();
    
    InsertPhiNodes();
    
    BuildSSAForm();
    
    
    for (auto it = cfg->basicBlocks.begin(); it != cfg->basicBlocks.end(); ++it) {
        BasicBlock *bb = *it;
        string succs_enumeration = "";
        for (auto const &s : bb->succs) { succs_enumeration += s->stringValue() + " "; }
        
        
        string preds_enumeration = "";
        for (auto const &s : bb->preds) { preds_enumeration += s->stringValue() + " "; }
        
        string dominator;
        if (bb->dominator) {
            dominator = bb->dominator->stringValue();
        } else {
            dominator = "";
        }
        printf("\n%s\n\t\tpreds: %s\n\t\tsuccs: %s\n\t\tdominatedBy: %s\n", bb->stringValue().c_str(), preds_enumeration.c_str(), succs_enumeration.c_str(), dominator.c_str());

        for (auto it = bb->statements.begin(); it != bb->statements.end(); ++it) {
            printf("\t%s\n", (*it)->Dump().c_str());
        }
    }
}


// MARK: Phi Nodes

void IRGenerator::InsertPhiNodes() {
    for (auto it : bblocksForVar) {
        std::string variableName = it.first;
        std::set<BasicBlock *> assignedInBlocks = it.second;
        std::set<BasicBlock *> dominanceFrontier = cfg->GetDominanceFrontierForSubSet(assignedInBlocks);
        for (auto bb : dominanceFrontier) {
            std::map<BasicBlock *, VariableExpession *> bbToVarMap;
        
            for (auto pred : bb->preds) {
                bbToVarMap[pred] = new VariableExpession(variableName);
            }
            PhiNodeStatement *phiStmt = new PhiNodeStatement(new VariableExpession(variableName), bbToVarMap);
            bb->statements.insert(bb->statements.begin(), phiStmt);
        }
    }
}

// MARK: VAR SEARCH

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

std::set<VariableExpession *> VarSearchVisitor::AllVarsUsedInStatement(AbstractStatement *statement){
    vars.clear();
    statement->Accept(this);
    return vars;
}


void VarSearchVisitor::Visit(BranchStatement *stmt) {
    if (stmt->condition)
        stmt->condition->Accept(this);
}
void VarSearchVisitor::Visit(AssignStatement *stmt) {
    stmt->rhs->Accept(this);
}
void VarSearchVisitor::Visit(NumberExpression *exp) {}
void VarSearchVisitor::Visit(VariableExpession *exp) { vars.insert(exp); }
void VarSearchVisitor::Visit(AssignExpression *exp) {
    vars.insert(exp->varExp);
    exp->expr->Accept(this);
}
void VarSearchVisitor::Visit(IfExpression *exp) {
    // On current version IfExpression can't be placed in ASSIGN statements
    assert("ForExpression can not be reached by VarSearchVisitor.");
    exp->conditionExp->Accept(this);
    exp->thenExp->Accept(this);
    exp->elseExp->Accept(this);
}
void VarSearchVisitor::Visit(BinaryExpression *exp) {
    exp->lhs->Accept(this);
    exp->rhs->Accept(this);
}
void VarSearchVisitor::Visit(ForExpression *exp) {
    // On current version ForExpr can't be placed in ASSIGN statements
    assert("ForExpression can not be reached by VarSearchVisitor.");
    vars.insert(exp->index);
    exp->end->Accept(this);
    exp->body->Accept(this);
    exp->start->Accept(this);
}

// MARK: SSA Form

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


void IRGenerator::BuildSSAForm() {
    SSAFormer ssaFormer = SSAFormer(cfg);
    for (auto pair : bblocksForVar) {
        std::string var = pair.first;
        ssaFormer.RenameVarToSSAForm(var);
    }
}


void SSAFormer::RenameVarToSSAForm(std::string varName) {
    counter = 0;
    stack.clear();
    TraverseBBWithVar(cfg->basicBlocks.front(), varName);
}

void SSAFormer::TraverseBBWithVar(BasicBlock *bb, std::string varName) {
    for (auto stmt : bb->statements) {
        // Renaming  vars in all rhs
        if (stmt->type != PHI) {
            set<VariableExpession *> vars = varSearcher.AllVarsUsedInStatement(stmt);
            for (auto var : vars) {
                if (var->name == varName)
                    var->SetSSAIndex(stack.back());
            }
        }
        
        // Renaming  vars in all lhs
        if (stmt->type == ASSIGN) {
            AssignStatement *assignStmt = static_cast<AssignStatement *>(stmt);
            if (assignStmt->var->name == varName) {
                assignStmt->var->SetSSAIndex(counter);
                stack.push_back(counter);
                counter += 1;
            }
        }
        
        if (stmt->type == PHI) {
            PhiNodeStatement *phiStmt = static_cast<PhiNodeStatement *>(stmt);
            if (phiStmt->var->name == varName) {
                phiStmt->var->SetSSAIndex(counter);
                stack.push_back(counter);
                counter += 1;
            }
        }
    }
    
    for (auto succBB : bb->succs) {
        for (auto stmt : succBB->statements) {
            if (stmt->type == PHI) {
                PhiNodeStatement *phiStmt = static_cast<PhiNodeStatement *>(stmt);
                if (phiStmt->bbToVarMap.count(bb) && phiStmt->bbToVarMap[bb]->name == varName) {
                    phiStmt->bbToVarMap[bb]->SetSSAIndex(stack.back());
                }
            }
        }
    }
    
    for (auto child : bb->domimatingBlocks) {
        TraverseBBWithVar(child, varName);
    }
    
    for (auto statement : bb->statements) {
        if (statement->type == ASSIGN) {
            AssignStatement *assignStmt = static_cast<AssignStatement *>(statement);
            if (assignStmt->var->name == varName) {
                stack.pop_back();
            }
        }
    }
}
