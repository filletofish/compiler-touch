//
//  IRInstructions.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "IRInstructions.hpp"
#include "Expressions.hpp"


std::string AssignInstruction::Dump() {
    return var->name + " = " + rhs->stringValue;
}
