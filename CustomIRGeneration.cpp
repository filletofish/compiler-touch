//
//  CustomIRGeneration.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "CustomIRGeneration.hpp"
#include "BasicBlock.hpp"
#include "Expressions.hpp"
#include "IRInstructions.hpp"
#include "ControlFlowGraph.hpp"

using namespace std;

BasicBlock* CustomIRGenerationVisitor::CreateBB() {
    int nextIndex = (int)cfg->basicBlocks.size();
    BasicBlock *bb = new BasicBlock(nextIndex);
    cfg->AddBasicBlock(bb);
    return bb;
}

CustomIRGenerationVisitor::CustomIRGenerationVisitor(ControlFlowGraph *cfg) :cfg(cfg) {
    BasicBlock *bb = CreateBB();
    currentBB = bb;
    entryBB = bb;
}

int CustomIRGenerationVisitor::Visit(NumberExpression *exp) {
    return exp->value;
}

int CustomIRGenerationVisitor::Visit(VariableExpession *exp) {
    int* value = namedValues[exp->name];
    if (!value)
        return LogError("Unknown variable name");
    return *value;
}

int CustomIRGenerationVisitor::Visit(AssignExpression *exp) {
    bblocksForVar[exp->varExp->name].insert(currentBB);
    int assignValue = exp->expr->Accept(this);
    namedValues[exp->varName()] = &assignValue;
    currentBB->AddInstruction(new AssignInstruction(exp->varExp, exp->expr));
    return assignValue;
}

int CustomIRGenerationVisitor::Visit(IfExpression *exp) {
    BasicBlock *thenBB = CreateBB();
    BasicBlock *elseBB = CreateBB();
    BasicBlock *mergeBB = CreateBB();
    
    BasicBlock::AddLink(currentBB, thenBB);
    BasicBlock::AddLink(currentBB, elseBB);
    BasicBlock::AddLink(thenBB, mergeBB);
    BasicBlock::AddLink(elseBB, mergeBB);
    
    currentBB->AddInstruction(new BranchInstruction(exp->conditionExp, thenBB, elseBB));
    
    currentBB = thenBB;
    exp->thenExp->Accept(this);
    currentBB->AddInstruction(new BranchInstruction(mergeBB));
    
    currentBB = elseBB;
    exp->elseExp->Accept(this);
    currentBB->AddInstruction(new BranchInstruction(mergeBB));
    
    currentBB = mergeBB;
    
    return 0;
}

int CustomIRGenerationVisitor::Visit(ForExpression *exp) {
    // TODO: Refactor to make less pseudo steps and to copy var instead of creating new
    
    bblocksForVar[exp->index->name].insert(currentBB);
    // Emit the start code first, without 'variable' in scope.
    int startVal = exp->start->Accept(this);
    VariableExpession *pseudoVarForStart = new VariableExpession(exp->index->name);
    currentBB->AddInstruction(new AssignInstruction(pseudoVarForStart, exp->start));

    int *oldVal = namedValues[exp->index->name];
    namedValues[exp->index->name] = &startVal;
    
    
    BasicBlock *loopCoonditionBB = CreateBB();
    BasicBlock::AddLink(currentBB, loopCoonditionBB);
    currentBB->AddInstruction(new BranchInstruction(loopCoonditionBB));
    currentBB = loopCoonditionBB;
    

    // Compute the end condition.
    bblocksForVar[exp->index->name].insert(currentBB);
    exp->end->Accept(this);
    // Make the new basic block for the loop body
    BasicBlock *LoopBB = CreateBB();
    BasicBlock *AfterBB = CreateBB();
    BasicBlock::AddLink(loopCoonditionBB, LoopBB);
    BasicBlock::AddLink(loopCoonditionBB, AfterBB);
    // MARK: Make pseudo step it in expression
    VariableExpession *pseudoVarForConditionCheck = new VariableExpession(exp->index->name);
    BinaryExpression *pseudoCompExp = new BinaryExpression('-', exp->end, pseudoVarForConditionCheck);
    currentBB->AddInstruction(new BranchInstruction(pseudoCompExp, LoopBB, AfterBB));
    
    
    
    // Start insertion in LoopBB.
    currentBB = LoopBB;
    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    exp->body->Accept(this);
    
    // MARK: Make pseudo step it in expression
    bblocksForVar[exp->index->name].insert(currentBB);
    BinaryExpression *pseudoStepExp = new BinaryExpression('+', new VariableExpession(exp->index->name), new NumberExpression(1));
    currentBB->AddInstruction(new AssignInstruction(exp->index, pseudoStepExp));
    currentBB->AddInstruction(new BranchInstruction(loopCoonditionBB));
    BasicBlock::AddLink(LoopBB, loopCoonditionBB);

    // Any new code will be inserted in AfterBB.
    currentBB = AfterBB;
    
    // Restore the unshadowed variable.
    if (oldVal)
        namedValues[exp->index->name] = oldVal;
    else
        namedValues.erase(exp->index->name);
    
    // for expr always returns 0.
    return 0;
}

int CustomIRGenerationVisitor::Visit(BinaryExpression *exp) {
    int lhsValue = exp->lhs->Accept(this);
    int rhsValue = exp->rhs->Accept(this);
    
    switch (exp->op) {
        case '+':
            return lhsValue + rhsValue;
        case '-':
            return lhsValue - rhsValue;
        default:
            return LogError("invalid binary operator");
    };
}

int CustomIRGenerationVisitor::LogError(const char * str) {
    fprintf(stderr, "Error: %s\n", str);
    return 0;
}


void CustomIRGenerationVisitor::Dump() {
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

        for (auto it = bb->instructions.begin(); it != bb->instructions.end(); ++it) {
            printf("\t%s\n", (*it)->Dump().c_str());
        }
    }
}


// MARK: Phi Nodes

void CustomIRGenerationVisitor::InsertPhiNodes() {
    for (auto it : bblocksForVar) {
        std::string variableName = it.first;
        std::set<BasicBlock *> assignedInBlocks = it.second;
        std::set<BasicBlock *> dominanceFrontier = cfg->GetDominanceFrontierForSubSet(assignedInBlocks);
        for (auto bb : dominanceFrontier) {
            std::map<BasicBlock *, VariableExpession *> bbToVarMap;
        
            for (auto pred : bb->preds) {
                bbToVarMap[pred] = new VariableExpession(variableName);
            }
            PhiInstruction *phiInstr = new PhiInstruction(new VariableExpession(variableName), bbToVarMap);
            bb->instructions.insert(bb->instructions.begin(), phiInstr);
        }
    }
}

// MARK: VAR SEARCH

std::set<VariableExpession *> VarSearchVisitor::AllVarsUsedInInstruction(AbstractInstruction *instruction){
    vars.clear();
    instruction->Accept(this);
    return vars;
}


void VarSearchVisitor::Visit(BranchInstruction *instr) {
    if (instr->condition)
        instr->condition->Accept(this);
}
void VarSearchVisitor::Visit(AssignInstruction *instr) {
    instr->rhs->Accept(this);
}
void VarSearchVisitor::Visit(NumberExpression *exp) {}
void VarSearchVisitor::Visit(VariableExpession *exp) { vars.insert(exp); }
void VarSearchVisitor::Visit(AssignExpression *exp) {
    vars.insert(exp->varExp);
    exp->expr->Accept(this);
}
void VarSearchVisitor::Visit(IfExpression *exp) {
    // On current version IfExpression can't be placed in ASSIGN Instructions
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
    // On current version ForExpr can't be placed in ASSIGN Instructions
    assert("ForExpression can not be reached by VarSearchVisitor.");
    vars.insert(exp->index);
    exp->end->Accept(this);
    exp->body->Accept(this);
    exp->start->Accept(this);
}

// MARK: SSA Form

void CustomIRGenerationVisitor::BuildSSAForm() {
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
    for (auto instr : bb->instructions) {
        // Renaming  vars in all rhs
        if (instr->type != PHI) {
            set<VariableExpession *> vars = varSearcher.AllVarsUsedInInstruction(instr);
            for (auto var : vars) {
                if (var->name == varName)
                    var->SetSSAIndex(stack.back());
            }
        }
        
        // Renaming  vars in all lhs
        if (instr->type == ASSIGN) {
            AssignInstruction *assignInstr = static_cast<AssignInstruction *>(instr);
            if (assignInstr->var->name == varName) {
                assignInstr->var->SetSSAIndex(counter);
                stack.push_back(counter);
                counter += 1;
            }
        }
        
        if (instr->type == PHI) {
            PhiInstruction *phiInstr = static_cast<PhiInstruction *>(instr);
            if (phiInstr->var->name == varName) {
                phiInstr->var->SetSSAIndex(counter);
                stack.push_back(counter);
                counter += 1;
            }
        }
    }
    
    for (auto succBB : bb->succs) {
        for (auto instr : succBB->instructions) {
            if (instr->type == PHI) {
                PhiInstruction *phiInstr = static_cast<PhiInstruction *>(instr);
                if (phiInstr->bbToVarMap.count(bb) && phiInstr->bbToVarMap[bb]->name == varName) {
                    phiInstr->bbToVarMap[bb]->SetSSAIndex(stack.back());
                }
            }
        }
    }
    
    for (auto child : cfg->GetChildrenForDominator(bb)) {
        TraverseBBWithVar(child, varName);
    }
    
    for (auto statement : bb->instructions) {
        if (statement->type == ASSIGN) {
            AssignInstruction *assignInstr = static_cast<AssignInstruction *>(statement);
            if (assignInstr->var->name == varName) {
                stack.pop_back();
            }
        }
        
//        if (statement->type == PHI) {
//            PhiInstruction *phiInstr = static_cast<PhiInstruction *>(statement);
//            if (phiInstr->var->name == varName) {
//                stack.pop_back();
//            }
//        }
    }
}
