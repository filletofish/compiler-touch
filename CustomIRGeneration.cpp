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
    // Emit the start code first, without 'variable' in scope.
    int startVal = exp->start->Accept(this);
    
    int *oldVal = namedValues[exp->index->name];
    namedValues[exp->index->name] = &startVal;
    
    
    
    BasicBlock *loopCoonditionBB = CreateBB();
    BasicBlock::AddLink(currentBB, loopCoonditionBB);
    currentBB = loopCoonditionBB;
    
    // Compute the end condition.
    // Make the new basic block for the loop body
    BasicBlock *LoopBB = CreateBB();
    BasicBlock *AfterBB = CreateBB();
    // MARK: Make pseudo step it in expression
    BinaryExpression *pseudoCompExp = new BinaryExpression('-', exp->end, exp->index);
    currentBB->AddInstruction(new BranchInstruction(pseudoCompExp, LoopBB, AfterBB));
    
    
    
    // Start insertion in LoopBB.
    currentBB = LoopBB;
    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    exp->body->Accept(this);
    
    // MARK: Make pseudo step it in expression
    BinaryExpression *pseudoStepExp = new BinaryExpression('+', exp->index, new NumberExpression(1));
    currentBB->AddInstruction(new AssignInstruction(exp->index, pseudoStepExp));
    currentBB->AddInstruction(new BranchInstruction(loopCoonditionBB));
    
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
        printf("\n%s\n\t\tpreds: %s\n\t\tsuccs: %s\n", bb->stringValue().c_str(), preds_enumeration.c_str(), succs_enumeration.c_str());

        for (auto it = bb->instructions.begin(); it != bb->instructions.end(); ++it) {
            printf("\t%s\n", (*it)->Dump().c_str());
        }

    }
}
