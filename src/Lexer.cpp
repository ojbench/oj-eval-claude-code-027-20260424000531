#include "Lexer.hpp"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& input) : input_(input) {}

void Lexer::skipWhitespace() {
    while (pos_ < input_.size() && (input_[pos_] == ' ' || input_[pos_] == '\t' || input_[pos_] == '\r')) {
        pos_++;
    }
}

Token Lexer::nextToken() {
    skipWhitespace();
    if (pos_ >= input_.size()) return {TokenType::EOL, ""};

    char c = input_[pos_];
    if (std::isdigit(c)) return number();
    if (std::isalpha(c)) return identifier();

    pos_++;
    switch (c) {
        case '+': return {TokenType::PLUS, "+"};
        case '-': return {TokenType::MINUS, "-"};
        case '*': return {TokenType::STAR, "*"};
        case '/': return {TokenType::SLASH, "/"};
        case '=': return {TokenType::EQUAL, "="};
        case '<': return {TokenType::LESS, "<"};
        case '>': return {TokenType::GREATER, ">"};
        case '(': return {TokenType::LPAREN, "("};
        case ')': return {TokenType::RPAREN, ")"};
        case ',': return {TokenType::COMMA, ","};
        default: return {TokenType::UNKNOWN, std::string(1, c)};
    }
}

Token Lexer::identifier() {
    size_t start = pos_;
    while (pos_ < input_.size() && (std::isalnum(input_[pos_]) || input_[pos_] == '_')) {
        pos_++;
    }
    std::string text = input_.substr(start, pos_ - start);

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"LET", TokenType::LET}, {"PRINT", TokenType::PRINT}, {"INPUT", TokenType::INPUT},
        {"GOTO", TokenType::GOTO}, {"IF", TokenType::IF}, {"THEN", TokenType::THEN},
        {"END", TokenType::END}, {"FOR", TokenType::FOR}, {"TO", TokenType::TO},
        {"STEP", TokenType::STEP}, {"NEXT", TokenType::NEXT}, {"REM", TokenType::REM},
        {"LIST", TokenType::LIST}, {"CLEAR", TokenType::CLEAR}, {"RUN", TokenType::RUN},
        {"QUIT", TokenType::QUIT}, {"INDENT", TokenType::INDENT}, {"DEDENT", TokenType::DEDENT}
    };

    auto it = keywords.find(text);
    if (it != keywords.end()) {
        if (it->second == TokenType::REM) {
            // REM consumes the rest of the line
            std::string comment = input_.substr(pos_);
            pos_ = input_.size();
            return {TokenType::REM, comment};
        }
        return {it->second, text};
    }
    return {TokenType::IDENTIFIER, text};
}

Token Lexer::number() {
    size_t start = pos_;
    while (pos_ < input_.size() && std::isdigit(input_[pos_])) {
        pos_++;
    }
    std::string text = input_.substr(start, pos_ - start);
    return {TokenType::NUMBER, text, std::stoi(text)};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token t = nextToken();
        tokens.push_back(t);
        if (t.type == TokenType::EOL) break;
    }
    return tokens;
}
