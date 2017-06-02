//
//  BaseBlock.hpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef BasicBlock_hpp
#define BasicBlock_hpp

#include <stdio.h>
#include <vector>
#include <string>

class AbstractInstruction;
class BasicBlock;

/* In syntax analysis, a basic block is a straight-line code sequence with no branches in except to the entry and no branches out except at the exit.
 * Basic blocks form the vertices or nodes in a control flow graph.
 */

class BasicBlock {
private:
    int index;
public:
    std::string stringValue();
    std::vector<AbstractInstruction *> instructions;
    std::vector<BasicBlock *> succs;
    std::vector<BasicBlock *> preds;

    void AddInstruction(AbstractInstruction *instruction);
    void AddSuccessor(BasicBlock *bb);
    void AddPredcessor(BasicBlock *bb);
    
    BasicBlock(int index): index(index) {};
};

#endif /* BasicBlock_hpp */
