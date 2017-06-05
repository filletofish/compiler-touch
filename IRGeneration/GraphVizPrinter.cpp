//
//  GraphVizPrinter.cpp
//  Compiler
//
//  Created by Филипп Федяков on 05.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "GraphVizPrinter.hpp"
#include "../ControlFlowGraph/ControlFlowGraph.hpp"
#include "../ControlFlowGraph/BasicBlock.hpp"

void GraphVizPrinter::print() {
    printf("digraph G {\n");
    for (auto bb : _cfg->basicBlocks) {
        for (auto succ : bb->succs) {
            printf("\t\"%s\" -> \"%s\"\n", bb->stringValue().c_str(), succ->stringValue().c_str());
        }
    }
    printf("}\n");
}
