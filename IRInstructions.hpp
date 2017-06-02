//
//  IRInstructions.hpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef IRInstructions_hpp
#define IRInstructions_hpp

#include <stdio.h>
enum InstructionType {
    ASSIGN = 0,
    RETURN = 1,
    BRANCH = 2,
    PHI = 3
};

class AbstractInstruction {
    InstructionType type;
    AbstractInstruction(InstructionType type) : type(type) {};
};

#endif /* IRInstructions_hpp */
