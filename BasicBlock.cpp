//
//  BaseBlock.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "BasicBlock.hpp"

void BasicBlock::AddInstruction(AbstractInstruction *instruction) {
    instructions.push_back(instruction);
}

void BasicBlock::AddPredcessor(BasicBlock *bb) {
    if(std::find(preds.begin(), preds.end(), bb) != preds.end()) {
        preds.push_back(bb);
    }
}

void BasicBlock::AddSuccessor(BasicBlock *bb) {
    if(std::find(succs.begin(), succs.end(), bb) != succs.end()) {
        succs.push_back(bb);
    }
}

std::string BasicBlock::stringValue() {
    return "bb #" + std::to_string(index);
}
