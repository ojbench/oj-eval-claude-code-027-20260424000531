#pragma once
#include <map>
#include <memory>
#include <vector>
#include <iostream>
#include "VarState.hpp"

class Statement;

class Program {
public:
    void addStatement(int lineNum, std::unique_ptr<Statement> stmt);
    void removeStatement(int lineNum);
    void run(VarState& state);
    void list();
    void clear();
    void setPC(int lineNum) { pc_ = lineNum; }
    int getPC() const { return pc_; }
    void stop() { running_ = false; }

private:
    std::map<int, std::unique_ptr<Statement>> statements_;
    int pc_ = -1;
    bool running_ = false;
};
