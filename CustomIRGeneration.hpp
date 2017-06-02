//
//  CustomIRGeneration.hpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef CustomIRGeneration_hpp
#define CustomIRGeneration_hpp

#include <stdio.h>
#include <string>
#include <map>

class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class ControlFlowGraph;
class BasicBlock;



class CustomIRGenerationVisitor {
public:
    int Visit(NumberExpression *exp);
    int Visit(VariableExpession *exp);
    int Visit(AssignExpression *exp);
    /// generates IR for if condition
    int Visit(IfExpression *exp);
    int Visit(ForExpression *exp);
    int Visit(BinaryExpression *exp);
    
    void Dump();
    CustomIRGenerationVisitor(ControlFlowGraph *cfg);
    
private:
    std::map<std::string, int*> namedValues;
    BasicBlock *currentBB;
    BasicBlock *entryBB;
    BasicBlock *CreateBB();
    ControlFlowGraph *cfg;
    int LogError(const char*);
};

#endif /* CustomIRGeneration_hpp */
