//
//  LLVMIRGenerator.cpp
//  Compiler
//
//  Created by Филипп Федяков on 28.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "LLVMIRGenerator.hpp"
#include "../Parser/Expressions.hpp"
#include "../ControlFlowGraph/ControlFlowGraph.hpp"
#include "../ControlFlowGraph/BasicBlock.hpp"

using namespace llvm;

llvm::Value* LLVMIRGenerator::GenerateIR(AbstractExpression *exp) {
    exp->Accept(this);
    return _latestValue;
}

void LLVMIRGenerator::Visit(NumberExpression *exp){
    _latestValue = ConstantInt::get(*TheContext, APInt(32, exp->value, false));
}

void LLVMIRGenerator::Visit(VariableExpession *exp) {
    llvm::AllocaInst *alloca = namedValues[exp->name];
    if (!alloca) {
        LogError("Unknown variable name");
        return;
    }
    _latestValue = Builder->CreateLoad(alloca, exp->name.c_str());
}

void LLVMIRGenerator::Visit(BinaryExpression *exp) {
    llvm::Value *lhsValue = GenerateIR(exp->lhs);
    llvm::Value *rhsValue = GenerateIR(exp->rhs);
    if (!lhsValue || !rhsValue)
        return;
    
    switch (exp->op) {
        case '+':
            _latestValue = Builder->CreateFAdd(lhsValue, rhsValue, "addtmp");
            return;
        case '-':
            _latestValue = Builder->CreateFSub(lhsValue, rhsValue, "subtmp");
            return;
        default:
            return LogError("invalid binary operator");
    }
}

void LLVMIRGenerator::Visit(AssignExpression *exp) {
    llvm::Value *assignValue = GenerateIR(exp->expr);
    if (!assignValue)
        return;
    if (namedValues.count(exp->varName()) != 0) {
        llvm::AllocaInst *Alloca = namedValues[exp->varName()];
        Builder->CreateStore(assignValue, Alloca);
    } else {
        llvm::AllocaInst *Alloca = Builder->CreateAlloca(llvm::Type::getInt32Ty(*TheContext), 0, exp->varName().c_str());
        Builder->CreateStore(assignValue, Alloca);
        namedValues[exp->varName()] = Alloca;
    }
    _latestValue = assignValue;
}


void LLVMIRGenerator::Visit(IfExpression *exp) {
    llvm::Value *CondV = GenerateIR(exp->conditionExp);
    if (!CondV)
        return;
    
    CondV = Builder->CreateICmpNE(
                                 CondV, llvm::ConstantInt::get(*TheContext, llvm::APInt(32,0,false)), "ifcond");
    
    Function *TheFunction = Builder->GetInsertBlock()->getParent();
    
    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*TheContext, "then", TheFunction);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(*TheContext, "else");
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*TheContext, "ifcont");
    
    Builder->CreateCondBr(CondV, ThenBB, ElseBB);
    
    // creating temp variable for storing return value
    std::string temp_varName = "temp_var";
    llvm::AllocaInst *tempVarAlloca = Builder->CreateAlloca(llvm::Type::getInt32Ty(*TheContext), 0, "temp_var");
    
    
    
    // Генерируем значение.
    Builder->SetInsertPoint(ThenBB);
    
    llvm::Value *ThenV = GenerateIR(exp->thenExp);
    if (!ThenV)
    
        return;

    Builder->CreateStore(ThenV, tempVarAlloca);
    
    Builder->CreateBr(MergeBB);
    // Кодогенерация 'Then' может изменить текущий блок, обновляем ThenBB для PHI.
    ThenBB = Builder->GetInsertBlock();
    
    /// Генерируем блок else.
    TheFunction->getBasicBlockList().push_back(ElseBB);
    Builder->SetInsertPoint(ElseBB);
    
    llvm::Value *ElseV = GenerateIR(exp->elseExp);
    if (!ElseV)
        return;
    
    Builder->CreateStore(ElseV, tempVarAlloca);
    Builder->CreateBr(MergeBB);
    // Кодогенерация 'Else' может изменить текущий блок, обновляем ElseBB для PHI.
    // codegen of 'Else' can change the current block, update ElseBB for the PHI.
    ElseBB = Builder->GetInsertBlock();
    
    // Emit merge block.
    TheFunction->getBasicBlockList().push_back(MergeBB);
    Builder->SetInsertPoint(MergeBB);
    
    llvm::Value *resultValue = Builder->CreateLoad(tempVarAlloca, "temp_var");
    
    _latestValue = resultValue;
}


void LLVMIRGenerator::Visit(ForExpression *exp) {
    
    Function *TheFunction = Builder->GetInsertBlock()->getParent();
    
    // Create an alloca for the variable in the entry block.
    AllocaInst *Alloca =Builder->CreateAlloca(llvm::Type::getInt32Ty(*TheContext), 0, exp->index->name.c_str());
    
    // Emit the start code first, without 'variable' in scope.
    llvm::Value *StartVal = GenerateIR(exp->start);
    if (!StartVal)
        return;
    
    // Store the value into the alloca.
    Builder->CreateStore(StartVal, Alloca);
    //  If it shadows an existing variable, we have to restore it, so save it now.
    llvm::AllocaInst *OldVal = namedValues[exp->index->name];
    namedValues[exp->index->name] = Alloca;
        
    llvm::BasicBlock *loopCoonditionBB = llvm::BasicBlock::Create(*TheContext, "loopCoonditionBB", TheFunction);
    Builder->CreateBr(loopCoonditionBB);
    Builder->SetInsertPoint(loopCoonditionBB);

    // Compute the end condition.
    llvm::Value *EndCond = GenerateIR(exp->end);
    if (!EndCond)
        return;
    
    llvm::Value *CurVar = Builder->CreateLoad(Alloca, exp->index->name.c_str());
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
    if (!GenerateIR(exp->body))
        return;
    // Emit the step value.
    llvm::Value *StepVal = nullptr;
    if (exp->step) {
        StepVal = GenerateIR(exp->step);
        if (!StepVal)
            return;
    } else {
        // If not specified, use 1.0.
        StepVal = llvm::ConstantInt::get(*TheContext, llvm::APInt(32, 1, false));
    }
    // Reload, increment, and restore the alloca.  This handles the case where
    // the body of the loop mutates the variable.
    CurVar = Builder->CreateLoad(Alloca, exp->index->name.c_str());
    llvm::Value *NextVar = Builder->CreateFAdd(CurVar, StepVal, "nextvar");
    Builder->CreateStore(NextVar, Alloca);
    Builder->CreateBr(loopCoonditionBB);
    
    // Any new code will be inserted in AfterBB.
    Builder->SetInsertPoint(AfterBB);
    
    // Restore the unshadowed variable.
    if (OldVal)
        namedValues[exp->index->name] = OldVal;
    else
        namedValues.erase(exp->index->name);
    
    // for expr always returns 0.
    _latestValue = llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*TheContext));
}

void LLVMIRGenerator::LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    _latestValue = nullptr;
}












