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
#include <map>

class BasicBlock;

class ControlFlowGraph {
private:
    std::vector<BasicBlock *>* _bbInPredOrder;
    std::vector<BasicBlock *>* _bbInPostOrder;
    std::map<BasicBlock *, int> _bbVisitedMap;
    
    
    void PredOrderDFS(BasicBlock *bb);
    void PostOrderDFS(BasicBlock *bb);
    void DomDFS(BasicBlock *bb);
public:
    
    std::vector<BasicBlock *> basicBlocks;
    void ComputePredOrder();
    void ComputePostOrder();
    void ComputeDomTree();
    void AddBasicBlock(BasicBlock *bb);
};
#endif /* ControlFlowGraph_hpp */
