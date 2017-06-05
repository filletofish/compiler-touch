//
//  Parser.cpp
//  Compiler
//
//  Created by Филипп Федяков on 27.05.17.
//  Copyright © 2017 filletofish. All rights reserved.
//

#include "Parser.hpp"
#include "Expressions.hpp"
#include "../Lexer/Lexer.hpp"
#include "../Lexer/Token.hpp"

#include <string>

AbstractExpression* Parser::ParseCurrentExpression() {
    auto LHS = ParsePrimary();
    if (!LHS)
        return nullptr;
    return ParseBinOpRHS(LHS);
}

AbstractExpression* Parser::ParsePrimary() {
    switch (lexer->current->type) {
        case tok_identifier:
            return ParseIdentifierExpression();
        case tok_number:
            return ParseNumberExpression();
        case tok_if:
            return ParseIfExpression();
        case tok_for:
            return ParseForExpression();
        default: {
            return LogError("unknown token when expecting an expression");
        }
    }
}

bool isCharAtToken(char c, Token *token) {
    if (token->type != tok_single_symbol) return false;
    auto symbolToken = static_cast<SingleSymbolToken *>(token);
    return symbolToken->c == c;
}

AbstractExpression* Parser::ParseNumberExpression() {
    auto currToken = static_cast<ValueToken *>(lexer->current);
    auto result = new NumberExpression(currToken->value);
    lexer->GetNextToken(); // consuming number;
    return result;
}

AbstractExpression* Parser::ParseIdentifierExpression() {
    auto currToken = static_cast<VarToken *>(lexer->current);
    std::string varName = currToken->identifier;

    lexer->GetNextToken();
    
    if (lexer->current->type != tok_assign) // Simple variable ref.
        return new VariableExpession(varName);
    
    lexer->GetNextToken();
    // it will parse next variable as declaration
    
    auto varDecl = ParseCurrentExpression();
    if (!varDecl) {
        LogError("expected '='");
        return nullptr;
    }
    return new AssignExpression(new VariableExpession(varName), varDecl);
}

AbstractExpression* Parser::ParseBinOpRHS(AbstractExpression * LHS) {
    while (true) {
        if (lexer->current->type != tok_operation)
            return LHS;
        
        auto currTokenOperation = static_cast<OpToken *>(lexer->current);
        
        char op = currTokenOperation->op;
        lexer->GetNextToken();
        
        // Parse the primary expression after the binary operator.
        auto RHS = ParsePrimary();
        if (!RHS)
            return nullptr;
        // Merge LHS/RHS.
        LHS = new BinaryExpression(op, LHS, RHS);
    }
}

AbstractExpression* Parser::ParseIfExpression() {
    lexer->GetNextToken();
    
    auto condExp = ParseCurrentExpression();
    if (!condExp)
        return nullptr;
    
    if (lexer->current->type != tok_then) {
        LogError("expected 'then'");
        return nullptr;
    }
    
    lexer->GetNextToken();
    
    auto thenExp = ParseCurrentExpression();
    if (!thenExp)
        return nullptr;
    
    if (lexer->current->type != tok_else) {
        LogError("expected 'else'");
        return nullptr;
    }
    
    lexer->GetNextToken();
    
    auto elseExp = ParseCurrentExpression();
    if (!elseExp)
        return nullptr;
    
    return new IfExpression(condExp, thenExp, elseExp);
}

AbstractExpression* Parser::ParseForExpression() {
    lexer->GetNextToken();
    
    if (lexer->current->type != tok_identifier) {
        LogError("expected variable after keyword 'for'");
        return nullptr;
    }
    
    std::string idName = static_cast<VarToken *>(lexer->current)->identifier;
    lexer->GetNextToken();
    
    if (lexer->current->type != tok_assign) {
        LogError("expected '=' after for");
        return nullptr;
    }
    
    lexer->GetNextToken();
    auto start = ParseCurrentExpression();
    if (!start)
        return nullptr;
    if (!isCharAtToken(',', lexer->current)) {
        LogError("expected ',' after for start value");
        return nullptr;
    }
    
    lexer->GetNextToken();
    
    auto end = ParseCurrentExpression();
    if (!end)
        return nullptr;
    
    AbstractExpression* step = nullptr;
    if (isCharAtToken(';', lexer->current)) {
        lexer->GetNextToken();
        step = ParseCurrentExpression();
        if (!step)
            return nullptr;
    }
    
    if (lexer->current->type != tok_in) {
        LogError("expected 'in' after for");
        return nullptr;
    }
    
    lexer->GetNextToken();
    
    auto body = ParseCurrentExpression();
    if (!body)
        return nullptr;
    
    return new ForExpression(idName, start, end, step, body);
}


AbstractExpression* Parser::LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}


std::vector<AbstractExpression *> Parser::Parse() {
    lexer->GetNextToken();
    std::vector<AbstractExpression *> expressions;
    
    AbstractExpression *expr = nullptr;
//    llvm::Value *RetVal = nullptr;
    while (lexer->current->type != tok_eof) {
        expr = ParseCurrentExpression();
        
        if (expr) {
            expressions.push_back(expr);
//            RetVal = expr->codegen();
//            if (!RetVal) getNextToken();
        } else {
            lexer->GetNextToken();
            //return expressions;
        }
    }
    
    return expressions;
}
