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

class AbstractStatement;
class BasicBlock;

/* In syntax analysis, a basic block is a straight-line code sequence with no branches in except to the entry and no branches out except at the exit.
 * Basic blocks form the vertices or nodes in a control flow graph.
 */

class BasicBlock {
private:
    int _index;
    std::string _label;
public:
    std::string stringValue();
    std::vector<AbstractStatement *> statements;
    std::vector<BasicBlock *> succs;
    std::vector<BasicBlock *> preds;
    BasicBlock *dominator;

    void AddStatement(AbstractStatement *statement);
    static void AddLink(BasicBlock *pred, BasicBlock *succ);
    
    BasicBlock(int index, std::string label): _index(index), _label(label) {};
};

#endif /* BasicBlock_hpp */
