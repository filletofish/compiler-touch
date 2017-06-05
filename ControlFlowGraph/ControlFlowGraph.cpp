
//
//  ControlFlowGraph.cpp
//  Compiler
//
//  Created by Филипп Федяков on 02.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "ControlFlowGraph.hpp"
#include "BasicBlock.hpp"
#include <set>


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

void ControlFlowGraph::ComputeDominatorTree() {
    if (!_bbInPredOrder) ComputePredOrder();
    
    // compute dominators
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
    
    // compute chilrenMap 
    for (auto v : basicBlocks) {
        BasicBlock *dominator = v->dominator;
        if (dominator) {
            dominator->domimatingBlocks.push_back(v);
        }
    }
}

void ControlFlowGraph::DomDFS(BasicBlock *bb) {
    if (!_bbVisitedMap[bb]) {
        _bbVisitedMap[bb] = 1;
        for (auto next : bb->succs) { if (!_bbVisitedMap[next]) DomDFS(next);}
    }
}

void ControlFlowGraph::ComputeBaseDominanceFrontier() {
    if (_dominanceFrontier) { return;}
    _dominanceFrontier = new map<BasicBlock *, set<BasicBlock *>>;
    for (auto v : basicBlocks) {
        _dominanceFrontier->insert(pair<BasicBlock *, set<BasicBlock *>>(v, set<BasicBlock *>()));
    }
    for (auto x : *_bbInPostOrder) {
        for (auto y : x->succs) {
            if (y->dominator != x)
                _dominanceFrontier->at(x).insert(y);
        }
        
        for (auto z : x->domimatingBlocks) {
            for (auto y : _dominanceFrontier->at(z)) {
                if (y->dominator != x)
                    _dominanceFrontier->at(x).insert(y);
            }
        }
    }
}

set<BasicBlock *> ControlFlowGraph::GetMergedDominanceFrontierFromSubSet(set<BasicBlock *> subSet) {
    if (!_dominanceFrontier) ComputeBaseDominanceFrontier();
    
    set<BasicBlock *> mergedDF;
    for (auto v : subSet) {
        set<BasicBlock *> df = _dominanceFrontier->at(v);
        mergedDF.insert(df.begin(), df.end());
    }
    return mergedDF;
}

set<BasicBlock *> ControlFlowGraph::GetDominanceFrontierForSubSet(std::set<BasicBlock *> subSet) {
    set<BasicBlock *> result;
    set<BasicBlock *> dfp;
    bool hasChanged = true;
    dfp = GetMergedDominanceFrontierFromSubSet(subSet);
    while (hasChanged) {
        hasChanged = false;
        dfp.insert(subSet.begin(), subSet.end());
        dfp = GetMergedDominanceFrontierFromSubSet(dfp);
        if (result != dfp) {
            result = dfp;
            hasChanged = true;
        }
    }
    
    return  result;
}

void ControlFlowGraph::CommitAllChanges() {
    ComputePredOrder();
    ComputePostOrder();
    ComputeDominatorTree();
    ComputeBaseDominanceFrontier();
}

