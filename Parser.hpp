//
//  Parser.hpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <vector>

class AbstractExpression;
class Lexer;

class Parser {
public:
    AbstractExpression* ParseCurrentExpression();
    Parser(Lexer *lexer): lexer(lexer) {};
    std::vector<AbstractExpression *> Parse();
private:
    Lexer* lexer;
    AbstractExpression* ParseNumberExpression();
    AbstractExpression* ParseIdentifierExpression();
    AbstractExpression* ParseIfExpression();
    AbstractExpression* ParseForExpression();
    AbstractExpression* ParsePrimary();
    AbstractExpression* ParseBinOpRHS(AbstractExpression * LHS);
    
    AbstractExpression* LogError(const char *Str);
};
#endif /* Parser_hpp */

