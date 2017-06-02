//
//  IRLLVMGenerationVisitor.cpp
//  Compiler
//
//  Created by Филипп Федяков on 28.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "IRGeneration.hpp"
#include "Expressions.hpp"

using namespace llvm;

llvm::Value* IRLLVMGenerationVisitor::Visit(NumberExpression *exp){
    return ConstantInt::get(*TheContext, APInt(32, exp->value, false));
}

llvm::Value* IRLLVMGenerationVisitor::Visit(VariableExpession *exp) {
    llvm::AllocaInst *alloca = namedValues[exp->name];
    if (!alloca)
        return LogError("Unknown variable name");
    return Builder->CreateLoad(alloca, exp->name.c_str());
}

llvm::Value* IRLLVMGenerationVisitor::Visit(BinaryExpression *exp) {
    llvm::Value *lhsValue = exp->lhs->Accept(this);
    llvm::Value *rhsValue = exp->rhs->Accept(this);
    if (!lhsValue || !rhsValue)
        return nullptr;
    
    switch (exp->op) {
        case '+':
            return Builder->CreateFAdd(lhsValue, rhsValue, "addtmp");
        case '-':
            return Builder->CreateFSub(lhsValue, rhsValue, "subtmp");
        default:
            return LogError("invalid binary operator");
    }
}

llvm::Value* IRLLVMGenerationVisitor::Visit(AssignExpression *exp) {
    llvm::Value *assignValue = exp->expr->Accept(this);
    if (!assignValue)
        return nullptr;
    if (namedValues.count(exp->varName) != 0) {
        llvm::AllocaInst *Alloca = namedValues[exp->varName];
        Builder->CreateStore(assignValue, Alloca);
    } else {
        llvm::AllocaInst *Alloca = Builder->CreateAlloca(llvm::Type::getInt32Ty(*TheContext), 0, exp->varName.c_str());
        Builder->CreateStore(assignValue, Alloca);
        namedValues[exp->varName] = Alloca;
    }
    return assignValue;
}


llvm::Value* IRLLVMGenerationVisitor::Visit(IfExpression *exp) {
    llvm::Value *CondV = exp->conditionExp->Accept(this);
    if (!CondV)
        return nullptr;
    
    CondV = Builder->CreateICmpNE(
                                 CondV, llvm::ConstantInt::get(*TheContext, llvm::APInt(32,0,false)), "ifcond");
    
    Function *TheFunction = Builder->GetInsertBlock()->getParent();
    
    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*TheContext, "then", TheFunction);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(*TheContext, "else");
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*TheContext, "ifcont");
    
    Builder->CreateCondBr(CondV, ThenBB, ElseBB);
    
    // Генерируем значение.
    Builder->SetInsertPoint(ThenBB);
    
    llvm::Value *ThenV = exp->thenExp->Accept(this);
    if (!ThenV)
        return nullptr;
    
    Builder->CreateBr(MergeBB);
    // Кодогенерация 'Then' может изменить текущий блок, обновляем ThenBB для PHI.
    ThenBB = Builder->GetInsertBlock();
    
    /// Генерируем блок else.
    TheFunction->getBasicBlockList().push_back(ElseBB);
    Builder->SetInsertPoint(ElseBB);
    
    llvm::Value *ElseV = exp->elseExp->Accept(this);
    if (!ElseV)
        return nullptr;
    
    Builder->CreateBr(MergeBB);
    // Кодогенерация 'Else' может изменить текущий блок, обновляем ElseBB для PHI.
    // codegen of 'Else' can change the current block, update ElseBB for the PHI.
    ElseBB = Builder->GetInsertBlock();
    
    // Emit merge block.
    TheFunction->getBasicBlockList().push_back(MergeBB);
    Builder->SetInsertPoint(MergeBB);
    
    // returning phiNode for returning some value from IF expression
    llvm::PHINode *PN = Builder->CreatePHI(llvm::Type::getInt32Ty(*TheContext), 2, "iftmp");
    
    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);
    return PN;
}


llvm::Value* IRLLVMGenerationVisitor::Visit(ForExpression *exp) {
    
    Function *TheFunction = Builder->GetInsertBlock()->getParent();
    
    // Create an alloca for the variable in the entry block.
    AllocaInst *Alloca =Builder->CreateAlloca(llvm::Type::getInt32Ty(*TheContext), 0, exp->indexName.c_str());
    
    // Emit the start code first, without 'variable' in scope.
    llvm::Value *StartVal = exp->start->Accept(this);
    if (!StartVal)
        return nullptr;
    
    // Store the value into the alloca.
    Builder->CreateStore(StartVal, Alloca);
    //  If it shadows an existing variable, we have to restore it, so save it now.
    llvm::AllocaInst *OldVal = namedValues[exp->indexName];
    namedValues[exp->indexName] = Alloca;
        
    llvm::BasicBlock *loopCoonditionBB = llvm::BasicBlock::Create(*TheContext, "loopCoonditionBB", TheFunction);
    Builder->CreateBr(loopCoonditionBB);
    Builder->SetInsertPoint(loopCoonditionBB);

    // Compute the end condition.
    llvm::Value *EndCond = exp->end->Accept(this);
    if (!EndCond)
        return nullptr;
    
    llvm::Value *CurVar = Builder->CreateLoad(Alloca, exp->indexName.c_str());
    EndCond = Builder->CreateICmpSLT(CurVar, EndCond, "loopcond");

    // Make the new basic block for the loop body
    llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(*TheContext, "loop", TheFunction);
    llvm::BasicBlock *AfterBB = llvm::BasicBlock::Create(*TheContext, "afterloop", TheFunction);
    Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

    
    
    // Start insertion in LoopBB.
    Builder->SetInsertPoint(LoopBB);
    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    if (!exp->body->Accept(this))
        return nullptr;
    // Emit the step value.
    llvm::Value *StepVal = nullptr;
    if (exp->step) {
        StepVal = exp->step->Accept(this);
        if (!StepVal)
            return nullptr;
    } else {
        // If not specified, use 1.0.
        StepVal = llvm::ConstantInt::get(*TheContext, llvm::APInt(32, 1, false));
    }
    // Reload, increment, and restore the alloca.  This handles the case where
    // the body of the loop mutates the variable.
    CurVar = Builder->CreateLoad(Alloca, exp->indexName.c_str());
    llvm::Value *NextVar = Builder->CreateFAdd(CurVar, StepVal, "nextvar");
    Builder->CreateStore(NextVar, Alloca);
    Builder->CreateBr(loopCoonditionBB);
    
    // Any new code will be inserted in AfterBB.
    Builder->SetInsertPoint(AfterBB);
    
    // Restore the unshadowed variable.
    if (OldVal)
        namedValues[exp->indexName] = OldVal;
    else
        namedValues.erase(exp->indexName);
    
    // for expr always returns 0.
    return llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*TheContext));
}

llvm::Value* IRLLVMGenerationVisitor::LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}
