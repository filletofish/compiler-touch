//
//  AbstractVisitor.hpp
//  Compiler
//
//  Created by Филипп Федяков on 05.06.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef AbstractVisitor_hpp
#define AbstractVisitor_hpp

#include <stdio.h>
class NumberExpression;
class VariableExpession;
class AssignExpression;
class IfExpression;
class ForExpression;
class BinaryExpression;
class AbstractExpression;
class AbstractStatement;
class BranchStatement;
class AssignStatement;


class AbstractVisitor {
public:
    virtual void Visit(NumberExpression *exp) = 0;
    virtual void Visit(VariableExpession *exp) = 0;
    virtual void Visit(AssignExpression *exp) = 0;
    virtual void Visit(IfExpression *exp) = 0;
    virtual void Visit(ForExpression *exp) = 0;
    virtual void Visit(BinaryExpression *exp) = 0;
    virtual void Visit(BranchStatement *stmt) = 0;
    virtual void Visit(AssignStatement *stmt) = 0;
};
#endif /* AbstractVisitor_hpp */
