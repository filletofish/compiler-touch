//
//  Token.hpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#ifndef Token_hpp
#define Token_hpp

#include <stdio.h>
#include <string>

enum TokenType {
    tok_eof = -1,
    
    // commands
    tok_if = -2,
    tok_then = -3,
    tok_else = -4,
    tok_for = -5,
    tok_in = -6,
    
    // primary
    tok_identifier = -7,
    tok_number = -8,
    
    // operations
    tok_operation = -9,
    tok_assign = -10,
    tok_single_symbol = -11
};

class Token {
public:
    
    TokenType type;
    Token(TokenType type): type(type) {};
};

class ValueToken: public Token {
public:
    int value;
    ValueToken(int value) : Token(tok_number), value(value) {};
};

class VarToken: public Token {
public:
    std::string identifier;
    VarToken(std::string identifier) : Token(tok_identifier), identifier(identifier) {};
};

class OpToken: public Token {
public:
    char op;
    OpToken(char op) : Token(tok_operation), op(op) {};
};

class SingleSymbolToken: public Token {
public:
    char c;
    SingleSymbolToken(char c) : Token(tok_single_symbol), c(c) {};
};

class AssignSymbolToken: public Token {
public:
    char c;
    AssignSymbolToken(char assign) : Token(tok_assign), c(assign) {};
};


#endif /* Token_hpp */
