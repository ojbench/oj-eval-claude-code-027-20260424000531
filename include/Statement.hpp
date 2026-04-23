#pragma once
#include <string>
#include <vector>
#include <variant>
#include "Expression.hpp"
#include "Program.hpp"

class Statement {
public:
    virtual ~Statement() = default;
    virtual void execute(VarState& state, Program& program) const = 0;
    virtual std::string toString() const = 0;
};

class LetStatement : public Statement {
    std::string var_;
    std::unique_ptr<Expression> expr_;
public:
    LetStatement(std::string var, std::unique_ptr<Expression> expr) : var_(var), expr_(std::move(expr)) {}
    void execute(VarState& state, Program& program) const override {
        state.set(var_, expr_->evaluate(state));
    }
    std::string toString() const override { return "LET " + var_ + " = ..."; }
};

class PrintStatement : public Statement {
    std::vector<std::variant<std::unique_ptr<Expression>, std::string>> components_;
    bool trailingComma_;
public:
    PrintStatement(std::vector<std::variant<std::unique_ptr<Expression>, std::string>> components, bool trailingComma)
        : components_(std::move(components)), trailingComma_(trailingComma) {}
    void execute(VarState& state, Program& program) const override {
        for (const auto& comp : components_) {
            if (std::holds_alternative<std::string>(comp)) {
                std::cout << std::get<std::string>(comp);
            } else {
                std::cout << std::get<std::unique_ptr<Expression>>(comp)->evaluate(state);
            }
        }
        if (!trailingComma_) std::cout << std::endl;
        else std::cout << std::flush;
    }
    std::string toString() const override { return "PRINT ..."; }
};

class InputStatement : public Statement {
    std::string var_;
public:
    InputStatement(std::string var) : var_(var) {}
    void execute(VarState& state, Program& program) const override {
        int val;
        if (!(std::cin >> val)) return;
        state.set(var_, val);
    }
    std::string toString() const override { return "INPUT " + var_; }
};

class GotoStatement : public Statement {
    int lineNum_;
public:
    GotoStatement(int lineNum) : lineNum_(lineNum) {}
    void execute(VarState& state, Program& program) const override {
        program.setPC(lineNum_);
    }
    std::string toString() const override { return "GOTO " + std::to_string(lineNum_); }
};

class IfStatement : public Statement {
    std::unique_ptr<Expression> left_, right_;
    std::string op_;
    int targetLine_;
public:
    IfStatement(std::unique_ptr<Expression> left, std::string op, std::unique_ptr<Expression> right, int target)
        : left_(std::move(left)), right_(std::move(right)), op_(op), targetLine_(target) {}
    void execute(VarState& state, Program& program) const override {
        int l = left_->evaluate(state);
        int r = right_->evaluate(state);
        bool cond = false;
        if (op_ == "=") cond = (l == r);
        else if (op_ == "<") cond = (l < r);
        else if (op_ == ">") cond = (l > r);
        else if (op_ == "<>") cond = (l != r);
        else if (op_ == "<=") cond = (l <= r);
        else if (op_ == ">=") cond = (l >= r);

        if (cond) program.setPC(targetLine_);
    }
    std::string toString() const override { return "IF ... " + op_ + " ... THEN " + std::to_string(targetLine_); }
};

class EndStatement : public Statement {
public:
    void execute(VarState& state, Program& program) const override {
        program.stop();
    }
    std::string toString() const override { return "END"; }
};

class IndentStatement : public Statement {
public:
    void execute(VarState& state, Program& program) const override {
        state.enterScope();
    }
    std::string toString() const override { return "INDENT"; }
};

class DedentStatement : public Statement {
public:
    void execute(VarState& state, Program& program) const override {
        state.exitScope();
    }
    std::string toString() const override { return "DEDENT"; }
};

class RemStatement : public Statement {
    std::string comment_;
public:
    RemStatement(std::string comment) : comment_(comment) {}
    void execute(VarState& state, Program& program) const override {}
    std::string toString() const override { return "REM" + comment_; }
};
