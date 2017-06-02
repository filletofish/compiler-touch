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
#include <string>

class AbstractExpression;
class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class ControlFlowGraph;


enum InstructionType {
    ASSIGN = 0,
    RETURN = 1,
    BRANCH = 2,
    PHI = 3
};

class AbstractInstruction {
public:
    InstructionType type;
    virtual std::string Dump() = 0;
    AbstractInstruction(InstructionType type) : type(type) {};
};


class AssignInstruction: public AbstractInstruction {
public:
    VariableExpession *var;
    AbstractExpression *rhs;
    
    std::string Dump() override;
    AssignInstruction(VariableExpession *var, AbstractExpression *rhs) : AbstractInstruction(ASSIGN), var(var), rhs(rhs) {};
};


#endif /* IRInstructions_hpp */
