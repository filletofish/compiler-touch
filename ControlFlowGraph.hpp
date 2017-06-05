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
#include <set>

class BasicBlock;

class ControlFlowGraph {
private:
    std::vector<BasicBlock *>* _bbInPredOrder;
    std::vector<BasicBlock *>* _bbInPostOrder;
    std::map<BasicBlock *, int> _bbVisitedMap;
    std::map<BasicBlock *, std::set<BasicBlock *>>* _dominanceFrontier;
    
    void PredOrderDFS(BasicBlock *bb);
    void PostOrderDFS(BasicBlock *bb);
    void DomDFS(BasicBlock *bb);
    
    void ComputePredOrder();
    void ComputePostOrder();
    void ComputeDominatorTree();
    void ComputeBaseDominanceFrontier();
    std::set<BasicBlock *> GetMergedDominanceFrontierFromSubSet(std::set<BasicBlock *> subSet);
public:
    
    std::vector<BasicBlock *> basicBlocks;
    std::set<BasicBlock *> GetDominanceFrontierForSubSet(std::set<BasicBlock *> subSet);
    void CommitAllChanges();
    void AddBasicBlock(BasicBlock *bb);
};
#endif /* ControlFlowGraph_hpp */
