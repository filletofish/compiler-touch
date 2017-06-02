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

CustomIRGenerationVisitor::CustomIRGenerationVisitor(ControlFlowGraph *cfg) :cfg(cfg) {
    BasicBlock *bb = new BasicBlock(0);
    currentBB = bb;
    currentBB = bb;
    cfg->AddBasicBlock(bb);
}

int* CustomIRGenerationVisitor::Visit(NumberExpression *exp) {
    return &exp->value;
}

int* CustomIRGenerationVisitor::Visit(VariableExpession *exp) {
    int* value = namedValues[exp->name];
    if (!value)
        return LogError("Unknown variable name");
    return value;
}

int* CustomIRGenerationVisitor::Visit(AssignExpression *exp) {
    int *assignValue = exp->expr->Accept(this);
    if (!assignValue)
        return nullptr;
    currentBB->AddInstruction(new AssignInstruction(exp->varExp, exp->expr));
    return assignValue;
}

int* CustomIRGenerationVisitor::Visit(IfExpression *exp) {
    return nullptr;
}

int* CustomIRGenerationVisitor::Visit(ForExpression *exp) {
    return nullptr;
}

int* CustomIRGenerationVisitor::Visit(BinaryExpression *exp) {
    return nullptr;
}

int* CustomIRGenerationVisitor::LogError(const char *) {
    printf("Some error!!");
    return nullptr;
}


void CustomIRGenerationVisitor::Dump() {
    for (auto it = cfg->basicBlocks.begin(); it != cfg->basicBlocks.end(); ++it) {
        BasicBlock *bb = *it;
        for (auto it = bb->instructions.begin(); it != bb->instructions.end(); ++it) {
            printf("%s\n", (*it)->Dump().c_str());
        }

    }
}
