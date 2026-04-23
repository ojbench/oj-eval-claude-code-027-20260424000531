#include "Parser.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

Token Parser::peek() const {
    if (pos_ >= tokens_.size()) return {TokenType::EOL, ""};
    return tokens_[pos_];
}

Token Parser::next() {
    Token t = peek();
    if (t.type != TokenType::EOL) pos_++;
    return t;
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        pos_++;
        return true;
    }
    return false;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    Token t = next();
    switch (t.type) {
        case TokenType::LET: {
            Token var = next();
            if (var.type != TokenType::IDENTIFIER) throw std::runtime_error("EXPECTED IDENTIFIER");
            if (!match(TokenType::EQUAL)) throw std::runtime_error("EXPECTED =");
            return std::make_unique<LetStatement>(var.text, parseExpression());
        }
        case TokenType::PRINT: {
            std::vector<std::variant<std::unique_ptr<Expression>, std::string>> components;
            while (peek().type != TokenType::EOL) {
                if (peek().type == TokenType::UNKNOWN && peek().text != "<>" && peek().text != "<=" && peek().text != ">=") {
                    components.push_back(next().text);
                } else {
                    components.push_back(parseExpression());
                }
                if (match(TokenType::COMMA)) {
                    // Handled by next iteration
                }
            }
            return std::make_unique<PrintStatement>(std::move(components));
        }
        case TokenType::INPUT: {
            Token var = next();
            if (var.type != TokenType::IDENTIFIER) throw std::runtime_error("EXPECTED IDENTIFIER");
            return std::make_unique<InputStatement>(var.text);
        }
        case TokenType::GOTO: {
            Token line = next();
            if (line.type != TokenType::NUMBER) throw std::runtime_error("EXPECTED LINE NUMBER");
            return std::make_unique<GotoStatement>(line.value);
        }
        case TokenType::IF: {
            auto left = parseExpression();
            std::string op = "";
            Token opToken = next();
            if (opToken.type == TokenType::EQUAL) op = "=";
            else if (opToken.type == TokenType::LESS) op = "<";
            else if (opToken.type == TokenType::GREATER) op = ">";
            else if (opToken.type == TokenType::UNKNOWN) {
                if (opToken.text == "<>" || opToken.text == "<=" || opToken.text == ">=") {
                    op = opToken.text;
                }
            }
            if (op == "") throw std::runtime_error("EXPECTED COMPARISON OPERATOR");
            auto right = parseExpression();
            if (!match(TokenType::THEN)) throw std::runtime_error("EXPECTED THEN");
            Token line = next();
            if (line.type != TokenType::NUMBER) throw std::runtime_error("EXPECTED LINE NUMBER");
            return std::make_unique<IfStatement>(std::move(left), op, std::move(right), line.value);
        }
        case TokenType::END: return std::make_unique<EndStatement>();
        case TokenType::INDENT: return std::make_unique<IndentStatement>();
        case TokenType::DEDENT: return std::make_unique<DedentStatement>();
        case TokenType::REM: return std::make_unique<RemStatement>(t.text);
        default: throw std::runtime_error("UNKNOWN STATEMENT TYPE");
    }
}

std::unique_ptr<Expression> Parser::parseExpression() {
    auto left = term();
    while (true) {
        if (match(TokenType::PLUS)) {
            left = std::make_unique<BinaryExpression>('+', std::move(left), term());
        } else if (match(TokenType::MINUS)) {
            left = std::make_unique<BinaryExpression>('-', std::move(left), term());
        } else {
            break;
        }
    }
    return left;
}

std::unique_ptr<Expression> Parser::term() {
    auto left = factor();
    while (true) {
        if (match(TokenType::STAR)) {
            left = std::make_unique<BinaryExpression>('*', std::move(left), factor());
        } else if (match(TokenType::SLASH)) {
            left = std::make_unique<BinaryExpression>('/', std::move(left), factor());
        } else {
            break;
        }
    }
    return left;
}

std::unique_ptr<Expression> Parser::factor() {
    Token t = next();
    if (t.type == TokenType::NUMBER) return std::make_unique<ConstExpression>(t.value);
    if (t.type == TokenType::IDENTIFIER) return std::make_unique<VariableExpression>(t.text);
    if (t.type == TokenType::LPAREN) {
        auto expr = parseExpression();
        if (!match(TokenType::RPAREN)) throw std::runtime_error("EXPECTED )");
        return expr;
    }
    if (t.type == TokenType::MINUS) {
        // Handle unary minus
        return std::make_unique<BinaryExpression>('-', std::make_unique<ConstExpression>(0), factor());
    }
    throw std::runtime_error("EXPECTED EXPRESSION");
}
