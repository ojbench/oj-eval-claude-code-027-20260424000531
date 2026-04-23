#include "Program.hpp"
#include "Statement.hpp"
#include <algorithm>

void Program::addStatement(int lineNum, std::unique_ptr<Statement> stmt) {
    statements_[lineNum] = std::move(stmt);
}

void Program::removeStatement(int lineNum) {
    statements_.erase(lineNum);
}

void Program::run(VarState& state) {
    if (statements_.empty()) return;
    pc_ = statements_.begin()->first;
    running_ = true;

    while (running_) {
        auto it = statements_.find(pc_);
        if (it == statements_.end()) {
            running_ = false;
            break;
        }

        int currentPC = pc_;
        // Default next PC is the next statement in order
        auto nextIt = std::next(it);
        if (nextIt != statements_.end()) {
            pc_ = nextIt->first;
        } else {
            pc_ = -1; // No more statements
        }

        try {
            it->second->execute(state, *this);
            // If pc_ was not changed by execute (like GOTO), and was -1, then stop
            if (pc_ == currentPC) {
                // This shouldn't happen if we updated pc_ to nextIt->first
            }
            if (pc_ == -1) running_ = false;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            running_ = false;
        }
    }
}

void Program::list() {
    for (const auto& pair : statements_) {
        std::cout << pair.first << " " << pair.second->toString() << std::endl;
    }
}

void Program::clear() {
    statements_.clear();
    pc_ = -1;
    running_ = false;
}
