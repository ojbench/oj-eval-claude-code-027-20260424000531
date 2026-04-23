#pragma once
#include "Lexer.hpp"
#include "Statement.hpp"
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Expression> parseExpression();

private:
    std::vector<Token> tokens_;
    size_t pos_ = 0;

    Token peek() const;
    Token next();
    bool match(TokenType type);

    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
};
