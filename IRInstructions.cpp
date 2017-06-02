//
//  IRInstructions.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "IRInstructions.hpp"
#include "Expressions.hpp"
#include "BasicBlock.hpp"


std::string AssignInstruction::Dump() {
    return var->name + " = " + rhs->stringValue;
}

std::string BranchInstruction::Dump() {
    if (isConditional) {
        return "branch on: " + condition->stringValue + " to: " + firstBranchBB->stringValue() + " or: " + secondBranchBB->stringValue();
    } else {
        return "branch to: " + firstBranchBB->stringValue();
    }
    
}
