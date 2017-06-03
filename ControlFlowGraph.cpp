
//
//  ControlFlowGraph.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "ControlFlowGraph.hpp"
#include "BasicBlock.hpp"

using namespace std;

void ControlFlowGraph::AddBasicBlock(BasicBlock *bb) {
    basicBlocks.push_back(bb);
}

void ControlFlowGraph::ComputePredOrder() {
    if (!_bbInPredOrder) {
        _bbVisitedMap.clear();
        _bbInPredOrder = new vector<BasicBlock *>;
        PredOrderDFS(basicBlocks.front());
    }
}

void ControlFlowGraph::PredOrderDFS(BasicBlock *bb) {
    _bbVisitedMap[bb] = 1;
    _bbInPredOrder->push_back(bb);
    for (auto next : bb->succs) { if (!_bbVisitedMap[next]) PredOrderDFS(next); }
}

void ControlFlowGraph::ComputePostOrder() {
    if (!_bbInPostOrder) {
        _bbVisitedMap.clear();
        _bbInPostOrder = new vector<BasicBlock *>;
        PostOrderDFS(basicBlocks.front());
    }
}

void ControlFlowGraph::PostOrderDFS(BasicBlock *bb) {
    _bbVisitedMap[bb] = 1;
    for (auto next : bb->succs) { if (!_bbVisitedMap[next]) PostOrderDFS(next); }
    _bbInPostOrder->push_back(bb);
}

void ControlFlowGraph::ComputeDomTree() {
    if (!_bbInPredOrder) ComputePredOrder();
    
    for (auto it : *_bbInPredOrder) {
        _bbVisitedMap.clear();
        _bbVisitedMap[it] = 1;
        DomDFS(_bbInPredOrder->front());
        for (auto jt : * _bbInPredOrder) {
            if (!_bbVisitedMap[jt]) {
                jt->dominator = it;
            }
        }
    }
}

void ControlFlowGraph::DomDFS(BasicBlock *bb) {
    if (!_bbVisitedMap[bb]) {
        _bbVisitedMap[bb] = 1;
        for (auto next : bb->succs) { if (!_bbVisitedMap[next]) DomDFS(next);}
    }
}

