#include <iostream>
#include <string>
#include <vector>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "VarState.hpp"

int main() {
    Program program;
    VarState state;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        Lexer lexer(line);
        std::vector<Token> tokens = lexer.tokenize();

        if (tokens.empty() || tokens[0].type == TokenType::EOL) continue;

        try {
            if (tokens[0].type == TokenType::NUMBER) {
                // Statement with line number
                int lineNum = tokens[0].value;
                if (tokens.size() > 1 && tokens[1].type != TokenType::EOL) {
                    std::vector<Token> stmtTokens(tokens.begin() + 1, tokens.end());
                    Parser parser(stmtTokens);
                    program.addStatement(lineNum, parser.parseStatement());
                } else {
                    program.removeStatement(lineNum);
                }
            } else if (tokens[0].type == TokenType::REM) {
                // Ignore immediate REM
            } else {
                // Immediate command
                if (tokens[0].type == TokenType::RUN) {
                    state.clear();
                    program.run(state);
                } else if (tokens[0].type == TokenType::LIST) {
                    program.list();
                } else if (tokens[0].type == TokenType::CLEAR) {
                    program.clear();
                    state.clear();
                } else if (tokens[0].type == TokenType::QUIT) {
                    break;
                } else {
                    // Immediate statement
                    Parser parser(tokens);
                    auto stmt = parser.parseStatement();
                    stmt->execute(state, program);
                }
            }
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}
