#pragma once
#include "VarState.hpp"

class Expression {
public:
    virtual ~Expression() = default;
    virtual int evaluate(const VarState& state) const = 0;
};

class ConstExpression : public Expression {
    int value_;
public:
    ConstExpression(int value) : value_(value) {}
    int evaluate(const VarState& state) const override { return value_; }
};

class VariableExpression : public Expression {
    std::string name_;
public:
    VariableExpression(const std::string& name) : name_(name) {}
    int evaluate(const VarState& state) const override { return state.get(name_); }
};

class BinaryExpression : public Expression {
    char op_;
    std::unique_ptr<Expression> left_, right_;
public:
    BinaryExpression(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {}
    int evaluate(const VarState& state) const override {
        int l = left_->evaluate(state);
        int r = right_->evaluate(state);
        switch (op_) {
            case '+': return l + r;
            case '-': return l - r;
            case '*': return l * r;
            case '/':
                if (r == 0) throw std::runtime_error("DIVISION BY ZERO");
                return l / r;
            default: throw std::runtime_error("UNKNOWN OPERATOR");
        }
    }
};
