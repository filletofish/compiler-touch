//
//  ControlFlowGraph.hpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef ControlFlowGraph_hpp
#define ControlFlowGraph_hpp

#include <stdio.h>
#include <vector>

class BasicBlock;

class ControlFlowGraph {
public:
    std::vector<BasicBlock *> basicBlocks;
    void AddBasicBlock(BasicBlock *bb);
};
#endif /* ControlFlowGraph_hpp */
