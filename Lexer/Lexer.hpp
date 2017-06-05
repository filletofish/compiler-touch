//
//  Lexer.hpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef Lexer_hpp
#define Lexer_hpp

#include <stdio.h>
#include <vector>
class Token;

class Lexer {
public:
    std::vector<Token *>getAllTokens();
    Token* GetNextToken();
    Token* current;
private:
    Token* GetToken();
    int lastChar = ' ';
};
#endif /* Lexer_hpp */
