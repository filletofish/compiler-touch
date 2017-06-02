//
//  ControlFlowGraph.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "ControlFlowGraph.hpp"

void ControlFlowGraph::AddBasicBlock(BasicBlock *bb) {
    basicBlocks.push_back(bb);
}
