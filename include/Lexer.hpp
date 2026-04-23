#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

enum class TokenType {
    NUMBER, IDENTIFIER,
    PLUS, MINUS, STAR, SLASH, EQUAL,
    LESS, GREATER, LPAREN, RPAREN, COMMA,
    LET, PRINT, INPUT, GOTO, IF, THEN, END,
    FOR, TO, STEP, NEXT,
    REM, LIST, CLEAR, RUN, QUIT,
    INDENT, DEDENT,
    EOL, UNKNOWN
};

struct Token {
    TokenType type;
    std::string text;
    int value; // For NUMBER
};

class Lexer {
public:
    Lexer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string input_;
    size_t pos_ = 0;

    void skipWhitespace();
    Token nextToken();
    Token identifier();
    Token number();
};
