//
//  Lexer.cpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "Lexer.hpp"
#include "Token.hpp"
#include <string>
#include <cstdlib>

Token * Lexer::GetNextToken() {
    return current = GetToken();
}

Token * Lexer::GetToken() {
    std::string identifier;
    
    while (isspace(lastChar))
        lastChar = getchar(); 
    
    if (isalpha(lastChar)) {
        identifier = lastChar;
        while (isalnum((lastChar = getchar())))
            identifier += lastChar;
        
        if (identifier == "if") return new Token(tok_if);
        if (identifier == "then") return new Token(tok_then);
        if (identifier == "else") return new Token(tok_else);
        if (identifier == "for") return new Token(tok_for);
        if (identifier == "in") return new Token(tok_in);
        return new VarToken(identifier);
    }
    
    if (isdigit(lastChar)) {
        std::string NumStr;
        do {
            NumStr += lastChar;
            lastChar = getchar();
        } while (isdigit(lastChar));
        
        int val = strtod(NumStr.c_str(), 0);
        return new ValueToken(val);
    }
    
    if (lastChar == '+' || lastChar == '-') {
        int op = lastChar;
        lastChar = getchar();
        return new OpToken(op);
    }
    
    if (lastChar == '=') {
        int assignOp = lastChar;
        lastChar = getchar();
        return new AssignSymbolToken(assignOp);
    }
    
    if (lastChar == EOF) {
        lastChar = getchar();
        return new Token(tok_eof);
    }
    
    int someSymbol = lastChar;
    lastChar = getchar();
    return new SingleSymbolToken(someSymbol);
}

std::vector<Token *> Lexer::getAllTokens() {
    std::vector<Token *> tokens;
    Token *token = GetNextToken();
    while (token->type != tok_eof) {
        tokens.push_back(token);
        token = GetNextToken();
    }
    
    tokens.push_back(token);
    return tokens;
}
